#include "esp_camera.h"
#include "Arduino.h"
#include "FS.h"
#include "SD_MMC.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "driver/rtc_io.h"
#include <EEPROM.h>

#if defined(ESP32)
#include <WiFi.h>
#endif

#include <ESP_Mail_Client.h>

#define WIFI_SSID "Airbox-EF60" //enter the wifi name
#define WIFI_PASSWORD "7k752wj4" //enter the password of the wifi

#define SMTP_HOST "smtp.gmail.com"


#define SMTP_PORT esp_mail_smtp_port_465 //gmail stmp port, others can be used but with testing we found out that gmail is faster and easier

#define AUTHOR_EMAIL "espcampsut@gmail.com" // email for project 
#define AUTHOR_PASSWORD "favuhxqhtjvibiez" //app password for this project's email

#define RECIPIENT_EMAIL "espcampsut@gmail.com" //email for project

void smtpCallback(SMTP_Status status);

SMTPSession smtp;

#define EEPROM_SIZE 1

RTC_DATA_ATTR int bootCount = 0;

#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

#define PIR_SENSOR_PIN    GPIO_NUM_13

int pictureNumber = 0;

void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //¯\(ツ)/¯
  Serial.begin(115200);

  Serial.setDebugOutput(true);

  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  pinMode(PIR_SENSOR_PIN,OUTPUT);
  pinMode(4, INPUT);
  digitalWrite(4, LOW);
  rtc_gpio_hold_dis(GPIO_NUM_4);

  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA; // the following can be used for framesizes + QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  // Init Camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  Serial.println("Starting SD Card");

  delay(500);
  if(!SD_MMC.begin()){
    Serial.println("SD Card Mount Failed");
    
  }

  uint8_t cardType = SD_MMC.cardType();
  if(cardType == CARD_NONE){
    Serial.println("No SD Card attached");
    return;
  }

} 

void loop() {
  bool motionDetected = digitalRead(PIR_SENSOR_PIN) == HIGH;

  if (motionDetected) {
    Serial.println("Motion detected!");

    camera_fb_t * fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Camera capture failed");
      return;
    }

    // initialize EEPROM with predefined size
    EEPROM.begin(EEPROM_SIZE);
    pictureNumber = EEPROM.read(0) + 1;

    // Path where the new picture will be saved in SD Card
    String path = "/7ARAMI" + String(pictureNumber) + ".jpg";

    fs::FS &fs = SD_MMC;
    Serial.printf("Picture file name: %s\n", path.c_str());

    File file = fs.open(path.c_str(), FILE_WRITE);
    if (!file) {
      Serial.println("Failed to open file in writing mode");
    }
    else {
      file.write(fb->buf, fb->len); // image
      Serial.printf("Saved file to path: %s\n", path.c_str());
      EEPROM.write(0, pictureNumber);
      EEPROM.commit();
    }
    file.close();
    esp_camera_fb_return(fb);
    Serial.print("Connecting to AP");

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(200);
    }

    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.println();

    ///check network
    MailClient.networkReconnect(true);

    
    smtp.debug(1);

    //sendng results
    smtp.callback(smtpCallback);

    // declaring session for user credentials(sender)
    Session_Config config;

    //setting session
    config.server.host_name = SMTP_HOST;
    config.server.port = SMTP_PORT;
    config.login.email = AUTHOR_EMAIL;
    config.login.password = AUTHOR_PASSWORD;

   //this was a must to include due to the nature of the library ¯\(ツ)/¯
    config.login.user_domain = "";

   
  
    config.time.ntp_server = F("pool.ntp.org,time.nist.gov");
    config.time.gmt_offset = 3;
    config.time.day_light_offset = 0;

    //message declaration
    SMTP_Message message;

    /* Enable the chunked data transfer with pipelining for large message if server supported */
    message.enable.chunking = true;

    //email structure
    message.sender.name = F("ESP Alert System Mail");
    message.sender.email = AUTHOR_EMAIL;

    message.subject = F("Test sending camera image");
    message.addRecipient(F("user1"), RECIPIENT_EMAIL);

    message.html.content = F("<span style=\"color:#ff0000;\">Intruder(7arami....).</span><br/><br/><img src=\"cid:image-001\" alt=\"esp32 cam image\"  width=\"2048\" height=\"1536\">");

    //transfer thing
    message.html.transfer_encoding = Content_Transfer_Encoding::enc_7bit;

   
    //can use us-ascii as charset.
    message.html.charSet = F("utf-8");



    SMTP_Attachment att;

    cr.filename = F("camera.jpg");
    att.descr.mime = F("image/jpg");

    att.blob.data = fb->buf;
    att.blob.size = fb->len;

    att.descr.content_id = F("image-001"); // The content id (cid) of camera.jpg image in the src tag

    //code for image
    att.descr.transfer_encoding = Content_Transfer_Encoding::enc_base64;

    /* Add inline image to the message */
    message.addInlineImage(att);

    //connecting to server
    if (!smtp.connect(&config))
    {
        MailClient.printf("Connection error, Status Code: %d, Error Code: %d, Reason: %s\n", smtp.statusCode(), smtp.errorCode(), smtp.errorReason().c_str());
        return;
    }

    if (!smtp.isLoggedIn())
    {
        Serial.println("Error, Not yet logged in.");
    }
    else
    {
        if (smtp.isAuthenticated())
            Serial.println("Successfully logged in.");
        else
            Serial.println("Connected with no Auth.");
    }

    /* Start sending the Email and close the session */
    if (!MailClient.sendMail(&smtp, &message, true))
        MailClient.printf("Error, Status Code: %d, Error Code: %d, Reason: %s\n", smtp.statusCode(), smtp.errorCode(), smtp.errorReason().c_str());

  

    MailClient.printf("Free Heap: %d\n", MailClient.getFreeHeap());


    delay(100);

    // Turn off built in GPIO 4 led flash pin
    pinMode(4, OUTPUT);
    digitalWrite(4, LOW);
    rtc_gpio_hold_en(GPIO_NUM_4);

    esp_sleep_enable_ext0_wakeup(PIR_SENSOR_PIN, 0);
    Serial.println("Going to sleep now");
    delay(1000);
    esp_deep_sleep_start();
    Serial.println("debug");
  }
  else {
    Serial.println("No motion detected. Waiting...");

    //here delay was needed
    delay(100);
  }
}

void smtpCallback(SMTP_Status status)
{
    //status of stmp
    Serial.println(status.info());

    //print result
    if (status.success())
    {
        

        
        MailClient.printf("Message sent success: %d\n", status.completedCount());
        MailClient.printf("Message sent failed: %d\n", status.failedCount());
        
        for (size_t i = 0; i < smtp.sendingResult.size(); i++)
        {
            /* Get the result item */
            SMTP_Result result = smtp.sendingResult.getItem(i);

            

            MailClient.printf("Message No: %d\n", i + 1);
            MailClient.printf("Status: %s\n", result.completed ? "success" : "failed");
            MailClient.printf("Date/Time: %s\n", MailClient.Time.getDateTimeString(result.timestamp, "%B %d, %Y %H:%M:%S").c_str());
            MailClient.printf("Recipient: %s\n", result.recipients.c_str());
            MailClient.printf("Subject: %s\n", result.subject.c_str());
        }
        Serial.println("----------------\n");

        // memory usage will increase, thus this line of code to clear memory 
        smtp.sendingResult.clear();
    }
}