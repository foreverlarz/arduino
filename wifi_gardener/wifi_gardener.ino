#include <Wire.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"

// Define component locations
const int pin_sda = 12;
const int pin_scl = 13;
const int lcd_addr = 0x71;

// Define networking parameters and buffers
const char* wifi_ssid = "SSID";
const char* wifi_pass = "PASS";
const char* ntp_hostname = "time.nist.gov";
const unsigned int port_listen = 2390;
const unsigned int ntp_packet_size = 48;
byte packet_buffer[ntp_packet_size];
const int delay_wifi_status = 50; // milliseconds
const int delay_udp_parse = 10; // milliseconds

// Define NTP constants and initialize NTP variables
const unsigned long seventy_years = 2208988800UL; // 70 years of seconds
const int tz_adjust = -7 * (60 * 60); // PDT is UTC-7, hours converted to seconds
unsigned long time_ntp;
unsigned long time_unix;
unsigned long time_utc;
unsigned int time_utc_HH;
unsigned int time_utc_mm;
unsigned long time_local;
unsigned int time_local_HH;
unsigned int time_local_mm;

// Make output pretty with these variables
const int col_max = 32; // max columns
int col_loc = 0;

Adafruit_7segment matrix = Adafruit_7segment();
WiFiUDP udp;
IPAddress ntp_address;

void setup() {

  Wire.begin(pin_sda, pin_scl);
  matrix.begin(lcd_addr);
  matrix.writeDisplay();

  Serial.begin(115200);
  Serial.println("\n\n\nWelcome to LARZ's Wifi Gardener!");

  WiFi.begin(wifi_ssid, wifi_pass);
  Serial.print("Connecting to SSID ");
  Serial.print(wifi_ssid);
  Serial.println(".");
  
  col_loc = 0;
  while (WiFi.status() != WL_CONNECTED) {
    dot_loop_serial_print();
    delay(delay_wifi_status);
  }
  Serial.println();
  Serial.print("Wifi connected with IP address ");
  Serial.print(WiFi.localIP());
  Serial.println(".");

  udp.begin(port_listen);
  Serial.print("UDP listening on port ");
  Serial.print(udp.localPort());
  Serial.println(".");

  WiFi.hostByName(ntp_hostname, ntp_address);
  Serial.print("Resolved ");
  Serial.print(ntp_hostname);
  Serial.print(" to IP ");
  Serial.print(ntp_address);
  Serial.println(".");

  matrix.drawColon(1);
  matrix.writeDisplay();

}

void loop() {
  
  time_ntp = get_ntp_time(ntp_address);
  // time_utc is a Unix timestamp (epoch is 70 years after NTP time)
  time_utc = time_ntp - seventy_years;
  time_utc_HH = time_unix_h(time_utc);
  time_utc_mm = time_unix_m(time_utc);
  Serial.print("The UTC time is ");
  time_serial_print(time_utc_HH, time_utc_mm);
  // time_local is a Unix timestamp 
  time_local = time_utc + tz_adjust;
  time_local_HH = time_unix_h(time_local);
  time_local_mm = time_unix_m(time_local);
  Serial.print("The local time is ");
  time_serial_print(time_local_HH, time_local_mm);

  time_7seg_print(matrix, time_local_HH, time_local_mm);

  delay(1000);

}

int dot_loop_serial_print() {
    if (col_loc < col_max) {
      Serial.print(".");
      col_loc++;
    } else {
      Serial.println(".");
      col_loc = 0;
    }
};

int time_unix_h(int timestamp) {
  return (timestamp % 86400L) / 3600;
}
int time_unix_m(int timestamp) {
  return (timestamp % 3600) / 60;
}

int time_serial_print(int HH, int mm) {
  if ( HH < 10 ) {
    Serial.print('0');
  }
  Serial.print(HH);
  Serial.print(":");
  if ( mm < 10 ) {
    Serial.print('0');
  }
  Serial.println(mm);
}

int time_7seg_print(Adafruit_7segment &matrix, int HH, int mm) {
  matrix.writeDigitNum(0, int(HH/10));
  matrix.writeDigitNum(1, HH % 10);
  matrix.drawColon(1);
  matrix.writeDigitNum(3, int(mm/10));
  matrix.writeDigitNum(4, mm % 10);
  matrix.writeDisplay();
}

int get_ntp_time(IPAddress &ntp_address) {
  send_ntp_packet(ntp_address);
  int time_pkt_sent = micros();
  Serial.print("Sent NTP packet to ");
  Serial.print(ntp_address);
  Serial.println(".");

  Serial.print("Waiting for NTP response from ");
  Serial.print(ntp_address);
  Serial.println(".");
  int pkt_length = udp.parsePacket();
  col_loc = 0;
  while (pkt_length == 0) {
    delay(delay_udp_parse);
    pkt_length = udp.parsePacket();
    dot_loop_serial_print();
  }
  int time_pkt_recd = micros();
  int time_elapsed = time_pkt_recd - time_pkt_sent;
  Serial.println();
  Serial.print("Received packet of length ");
  Serial.print(pkt_length);
  Serial.print(" after ");
  // convert microseconds to milliseconds
  Serial.print(time_elapsed/1000);
  Serial.print(" ms (res<");
  Serial.print(delay_udp_parse);
  Serial.println(").");

  // read the packet into the buffer
  udp.read(packet_buffer, ntp_packet_size);

  // The NTP transmit timestamp is 64 bits (8 bytes or 4 words) long.
  // Transmit timestamp is located at bytes 40 - 47.
  // The first 4 bytes (2 words) are the integer part.
  // Ref: https://tools.ietf.org/html/rfc958#appendix-B
  // Extract these 4 bytes, cast into two words.
  unsigned long word_high = word(packet_buffer[40], packet_buffer[41]);
  unsigned long word_low = word(packet_buffer[42], packet_buffer[43]);
  // Combine the 2 words into a long int (seconds since 1900-01-01)
  unsigned long time_ntp = word_high << 16 | word_low;

  return time_ntp;
};

int send_ntp_packet(IPAddress &ntp_address) {
  // set all bytes in the buffer to 0
  memset(packet_buffer, 0, ntp_packet_size);
  // Initialize values needed to form NTP request
  packet_buffer[0] = 0b11100011; // LI, Version, Mode
  packet_buffer[1] = 0;          // Stratum, or type of clock
  packet_buffer[2] = 6;          // Polling Interval
  packet_buffer[3] = 0xEC;       // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packet_buffer[12]  = 49;
  packet_buffer[13]  = 0x4E;
  packet_buffer[14]  = 49;
  packet_buffer[15]  = 52;
  // All NTP fields have values assigned, now send the NTP request to port 123
  udp.beginPacket(ntp_address, 123);
  udp.write(packet_buffer, ntp_packet_size);
  udp.endPacket();
}
