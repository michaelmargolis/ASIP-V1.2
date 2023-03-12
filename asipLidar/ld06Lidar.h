/* ld06Lidar.h
   code to read LdRobot LD06 lidar and send distance data over serial
   Tested on Pi Pico but should work on other 32 bit boards with free hardware serial port

  All messages begin with '@R' and are terminated with newline 

   Note that range data is sent as millimeters and the rotation order of range data is clockwise

  Message format:   
    Range message:
    range_header: "@Nr", 
    scan_id, (value must match value in descriptor message) 
    range_id, (must be one more than previous msg for this scan)  
    ranges (mm) (comma seperated distances terminated by newline)

    Range terminator message:
    header: "@Rt",
    scan_number,  (value must match previous range msgs 
    angle_min, (angle*100)
    angle_max, (andgle*100)
    range_min, (mm)
    range_max, (mm)
    range_len (this must match the number of range values)
    newline terminator
   See  show_lidar.py for an example consumer of this data
   */

#include <vector>

enum {
  PKG_HEADER = 0x54,
  PKG_VER_LEN = 0x2C,
  POINT_PER_PACK = 12,
  ANGLE_HEADER = 'A',  // header for comma separated angle values
  DISTANCE_HEADER = 'D',   // header for comma separated distance values
};

#define RANGE_HEADER "@Nr"  // distance data for part of one scan
#define RANGE_TERM_HEADER "@Nt"  // message following range msgs for one complete scan

// the following two structs represent data sent from LD06
typedef struct __attribute__((packed)) {
  uint16_t distance;
  uint8_t confidence;
} LidarPointStructDef;

typedef struct __attribute__((packed)) {
  uint8_t header;
  uint8_t ver_len;
  uint16_t speed;
  uint16_t FSA;  // start_angle;
  LidarPointStructDef point[POINT_PER_PACK];
  uint16_t LSA;  //end_angle;
  uint16_t timestamp;
  uint8_t crc8;
} LiDARFrameTypeDef;

LiDARFrameTypeDef LidarFrame;

#define LIDAR_SERIAL_PORT Serial1
#define NBR_CALIBRATION_SCANS  10 // data for these scans not sent

class LD06 {

private:  
  int sequence_id;
  //int16_t  pointsCount; // total nuber of points in this scan
  int32_t prev_LSA;
  HardwareSerial *lidarSerial; // connection to lidar
  Stream *stream;  // processed output data
 

public:
  int32_t scan_id;   // nbr of complete revolutions of the scanner
  int32_t angleMin; // start angle of scan in degrees * 100
  int32_t angleMax; // end angle of scan in degrees * 100

  int rangeMin = 0; // todo min and max are not yet populated
  int rangeMax = 0;
  int rangeCount = 0; // total number of distances sent in this scan
  std::vector<uint16_t> ranges;
  bool outputEnabled = true;

  void begin(HardwareSerial *LidarSerial, int8_t rxPin, int8_t tx_pin, Stream *outStream=&Serial) {
    lidarSerial = LidarSerial;
    lidarSerial->begin(230400);
    stream = outStream;    
    scan_id = -NBR_CALIBRATION_SCANS; // scans send when count > 0 
  };
  
  void setOutStream( Stream *outStream){
    stream = outStream;
  }

  void sendRangeMsg(int firstRangeIndex, int nbrRanges){
    if( scan_id > 0 && nbrRanges > 0) { // ignore initial calibration scans
      stream->printf("%s,%d,%d", RANGE_HEADER, scan_id, sequence_id);
      for (int i = firstRangeIndex; i < firstRangeIndex+nbrRanges; i++) {
        stream->printf(",%d", LidarFrame.point[i].distance);
      }
      stream->println();
      rangeCount +=  nbrRanges;
      sequence_id++;  
    }
  }

  void sendRangeScanEnd() {
    if( scan_id > 0){ // ignore initial calibration scans
      stream->printf("%s,%d,%d,%d,%d,%d,%d\n", RANGE_TERM_HEADER, scan_id, angleMin, angleMax, rangeMin, rangeMax, rangeCount);
    }
    scan_id++;
    sequence_id = 0;
    rangeCount = 0;
  }

  void calculate() {
    // note angle values from lidar are hundredths of a degree
    float span = LidarFrame.LSA - LidarFrame.FSA; // difference between last and first angle
    int startIndex; // if != 0, this is the start of data for the next scan
    float angle_step;
    if (span > 0) {
      // here if scan has not crossed back to 0
      angle_step = span / (POINT_PER_PACK - 1);
      if(LidarFrame.FSA < prev_LSA){
        //must have crossed just before this calculation
        angleMax = prev_LSA; 
        sendRangeScanEnd(); // send final msg for previous scan
        angleMin = LidarFrame.FSA; // store the first angle in the scan
        startIndex = 0; // all data is for a new scan
        // stream->printf("!New cycle, scan_id=%d, startIndex=%d, startAngle=%d endAngle=%d\n", scan_id, startIndex, LidarFrame.FSA ,prev_LSA);
        sendRangeMsg(0, POINT_PER_PACK); // send range msg for start of next scan
      }
      else{
        sendRangeMsg(0, POINT_PER_PACK); // send range msg for start of next scan
      }
    } else { 
      // here if scan crossed zero within the data packet    
 
      angle_step = (LidarFrame.LSA + (36000 - LidarFrame.FSA)) / (float)(POINT_PER_PACK - 1);
      span = angle_step * (POINT_PER_PACK - 1);
      startIndex = 0;
      while(++startIndex < POINT_PER_PACK) {
          // stream->printf("%d=%f, ", startIndex, LidarFrame.FSA + (startIndex * angle_step));
          if(LidarFrame.FSA + (startIndex * angle_step) > 36000) // break at xover
              break;
      }             
      sendRangeMsg(0, startIndex-1);  // send data prior to crossover 
      angleMax = (int)LidarFrame.FSA + ((startIndex-1)*angle_step);
      // stream->printf("!new cycle, scanCount=%d, startIndex=%d, startAngle=%d, endAngle=%d\n", scan_id, startIndex,  angleMin, angleMax);
      sendRangeScanEnd();
      angleMin = (int)(LidarFrame.FSA + (startIndex * angle_step)-36000); // this if for the next scan
      sendRangeMsg(startIndex, POINT_PER_PACK-startIndex ); // send data following crossover  
    }
    prev_LSA = LidarFrame.LSA;
    if (angle_step > 2000) {
      stream->printf("!MISSING DATA, step=%d", angle_step);
      return;  // return if step size is abnormal (more then 20 degrees )
    }
  }

  // read data and update scan dispaly
  void service() {
    if (lidarSerial->available() >= (int)sizeof(LidarFrame)) {
      if (lidarSerial->read() == PKG_HEADER) {
        int nbrPoints = lidarSerial->read() & 0x1F;
        if (nbrPoints == POINT_PER_PACK) {
          lidarSerial->readBytes((char *)&LidarFrame + 2, sizeof(LidarFrame) - 2);
          if(outputEnabled) {
            // only process data if output is enabled
            calculate();
          }             
        }
      }
    }
  }

};
