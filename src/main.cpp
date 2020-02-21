
#include <Adafruit_FONA.h>
#include <SoftwareSerial.h>

//IMPORTANT: either use 2 & 3 or 4 & 5 for the TX and RX respectively for Software Serial!
#define FONA_TX 4 //Soft serial port
#define FONA_RX 5 //Soft serial port
#define FONA_RST 9

#define FONA_POWER 8
#define FONA_POWER_ON_TIME 180   /* 180ms*/
#define FONA_POWER_OFF_TIME 1000 /* 1000ms*/

#define ever (;;)

#define SERIAL_BAUD 115200
#define FONA_BAUD 4800

#define CHECK_INTERVAL 1000

char replybuffer[255]; // this is a large buffer for replies
unsigned long timecounts = 0;
unsigned long last_timecounts = 0;

SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
Adafruit_FONA_3G fona = Adafruit_FONA_3G(FONA_RST);

void process_sms();
void delete_SMS_all();

void setup()
{
    //for initializing the GSM
    pinMode(FONA_POWER, OUTPUT);
    digitalWrite(FONA_POWER, HIGH);
    delay(FONA_POWER_ON_TIME);
    digitalWrite(FONA_POWER, LOW);
    delay(3000);

    while (!Serial); // wait till serial gets initialized

    Serial.begin(SERIAL_BAUD); // baud rate
    Serial.println(F("Sustaingineering 3G TxRx!!"));
    Serial.println(F("Initializing....(May take 3 seconds)"));

    fonaSS.begin(FONA_BAUD);
    while (!fona.begin(fonaSS))
    {
        Serial.println(F("Couldn't find FONA")); // reboot arduino and fona if this shows up! (Should probably do this automatically for robustness)
        delay(1000);
        for ever;
    }
    Serial.println(F("FONA is OK"));

    Serial.println(F("Searching for network\n"));
    bool SIMFound = false;
    for (int countdown = 600; countdown >= 0 && SIMFound == false; countdown--)
    {
        uint8_t n = fona.getNetworkStatus(); // constantly check until network is connected to home    sendCheckReply(F("AT+CLVL="), i, ok_reply);
        if (n == 1)
        {
            SIMFound = true;
            Serial.println(F("Found, network connected")); //If program hangs here, SIM card cannot be read/connect to network
            int num_sms = fona.getNumSMS();
            if (num_sms > 0)
            {
                delete_SMS_all();
            }
        }
    }

    if (!SIMFound)
    {
        Serial.println(F("SIM card could not be found. Please ensure that your SIM card is compatible with dual-band UMTS/HSDPA850/1900MHz WCDMA + HSDPA."));
        for ever;
    }
}

void loop()
{
    timecounts = millis();
    if (timecounts > last_timecounts + CHECK_INTERVAL)
    {
        last_timecounts = timecounts;
        process_sms();
    }
}

// Handle messages like a stack, we take the top and work down
void process_sms()
{
    int num_sms = fona.getNumSMS();
    while (num_sms > 0)
    {
        uint8_t smsn = num_sms - 1; // 0-th indexed

        Serial.print(F("SMS available: "));
        Serial.println(num_sms);
        Serial.print(F("Message index: "));
        Serial.println(smsn);

        uint16_t smslen;
        if (!fona.readSMS(smsn, replybuffer, 250, &smslen))
        {
            Serial.println(F("Failed getting SMS from FONA!"));
            delete_SMS_all();
            return;
        }

        Serial.println(replybuffer);

        if (fona.deleteSMS(smsn))
        {
            Serial.print(F("Deleted SMS index #"));
            Serial.println(smsn);
        }
        else
        {
            Serial.println(F("Couldn't delete, clearing all!"));
            delete_SMS_all();
            return;
        }
        num_sms = fona.getNumSMS();
    }
}

void delete_SMS_all()
{
    int max_SMS_num = 50;

    Serial.println(F("Clearing FONA message cache."));

    for (int i = 0; i < max_SMS_num; i++)
    {
        if (fona.deleteSMS(i))
        {
            Serial.print(F("Deleted SMS index #"));
            Serial.println(i);
        }
    }
    Serial.println(F("All messages are cleared"));
}
