# Title

- Design a system to send logs from ESP32 to Firebase and view on React application

## overview

- Use pure HTML and connect to firebase to display log data

- Create ESP 32 code to connect to firebase

- Allow configuring variables WIFI*SSID, WIFI_PASSWORD, FIREBASE_HOST, FIREBASE_AUTH from terminal when entering the character "#set_variable*-_{variable names}_-\_{variable values ​​in string format}" in ESP 32 and save to ROM so that the next time it starts, it will always read the saved variables, when pressing reset, the variables will not be deleted

- ESP32 sends any information received when the connection is completed from terminal to firebase

## Test Cases

### 1. WiFi Configuration Tests

a) Normal Operation:

- Test setting valid SSID and password
- Test connection to WPA2 networks
- Test connection to open networks
- Test persistence after power cycle

b) Error Cases:

- Invalid SSID/password combination
- Network out of range
- Invalid configuration command format
- Special characters in SSID/password

### 2. Firebase Configuration Tests

a) Connection:

- Valid Firebase host and auth
- Invalid Firebase credentials
- Connection timeout scenarios
- Reconnection after connection loss

b) Data Storage:

- Small log entries (< 1KB)
- Large log entries (> 100KB)
- Rapid succession of logs
- Special characters in log data

### 3. Terminal Command Tests

a) Configuration Commands:

- #set*variable_WIFI_SSID*"MyNetwork"
- #set*variable_WIFI_PASSWORD*"MyPassword"
- #set*variable_FIREBASE_HOST*"host-url"
- #set*variable_FIREBASE_AUTH*"auth-token"

b) Edge Cases:

- Empty values
- Very long values
- Special characters
- Malformed commands

### 4. Web Interface Tests

a) Display Functions:

- Real-time log updates
- Historical log loading
- Large dataset handling
- Different data formats

b) User Interaction:

- Filtering logs
- Searching logs
- Sorting logs
- Export functionality

### 5. System Stability Tests

a) Long-term Operation:

- 24-hour continuous operation
- Memory usage monitoring
- Connection stability
- Data integrity

b) Recovery Scenarios:

- Power loss recovery
- Network interruption recovery
- Firebase service interruption
- System reset behavior

### 6. Performance Tests

a) Data Throughput:

- Maximum sustainable log rate
- Multiple concurrent connections
- Large dataset handling
- Real-time update performance

b) Resource Usage:

- ESP32 memory utilization
- Battery consumption (if applicable)
- Network bandwidth usage
- Storage utilization

### 7. Security Tests

a) Configuration Protection:

- ROM data persistence
- Configuration command validation
- Credential protection
- Access control

b) Data Security:

- Data transmission security
- Storage security
- Access token management
- Input validation
