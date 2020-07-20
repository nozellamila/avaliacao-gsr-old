#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include <WiFi.h>
#include <Dns.h>

const char* ssid = "FIGUEIRAS";
const char* password = "dm1952999";

IPAddress server_addr(192,168,15,60);

char user[] = "milaip"; // MySQL user login username
char passwordSQL[] = "teste"; // MySQL user login password
char db[] = "cadastro";
char INSERT_SQL[] = "INSERT INTO SensorData(`sensor`, `location`,`value1`, `value2`, `value3`) VALUES ('sensor', 'sala', 11, 23, 45)";

WiFiClient client;            // Use this for WiFi instead of EthernetClient
MySQL_Connection conn((Client *)&client);

WiFiServer  server(80);
bool statusConnect;

void setup() {
  //Código de configuração aqui
  Serial.begin(115200);

  connectToNetwork();
  mySqlLoop();
}

void loop() {
  /*
  if (conn.connect(server_addr, 3306, user, passwordSQL, db)) {
    Serial.println("Database connected.");
  }
  else{
    Serial.println("Connection failed.");
    Serial.println("Recording data.");
  }
  */
}
void connectToNetwork() {
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Establishing connection to WiFi..");
  }
 
  Serial.println("Connected to network");
 
}
void mySqlLoop(){

  
  MySQL_Connection conn((Client *)&client);
  if (conn.connect(server_addr, 3306, user, passwordSQL, db)) {
    Serial.println("Database connected.");
  }
  else{
    Serial.println("Connection failed.");
    Serial.println("Recording data.");
  }
  // Initiate the query class instance
  MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
  // Execute the query
  cur_mem->execute(INSERT_SQL);
  // Note: since there are no results, we do not need to read any data
  // Deleting the cursor also frees up memory used
  delete cur_mem;
  Serial.println("closing connection\n");
  //client.stop();
  
}
