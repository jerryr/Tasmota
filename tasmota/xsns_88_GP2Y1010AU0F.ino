/**
 * TODO:
 * 1. Update all language files
 * 2. Make dust density calculation coefficients configurable by SensorXX commands
 * 
 **/

#ifdef USE_GP2Y1010AU0F
const uint16_t samplingTime = 280;
const uint8_t measurePin = A0;
const uint8_t deltaTime = 40;

#define XSNS_88 88

#ifdef USE_WEBSERVER
// {s} = <tr><th>, {m} = </th><td>, {e} = </td></tr>
const char HTTP_SNS_DUST_SENSOR[] PROGMEM = "{s} Dust Sensor {m}%_f{e}";
#endif  // USE_WEBSERVER

float dust_value;

void DustSensorShow(bool use_json){

    if(use_json){
            ResponseAppend_P(PSTR(",\"dustSensor\":%_f"), &dust_value);
    }
    #ifdef USE_WEBSERVER
    else{
            WSContentSend_P(HTTP_SNS_DUST_SENSOR, &dust_value);
    }
    #endif

}

void DustSensorEverySecond() {
  char log_data[50];
  uint8_t ledPin = Pin(GPIO_GP2Y1010AU0F);

  ext_snprintf_P(log_data, sizeof(log_data), "ledPin = %d", ledPin);
  AddLog(LOG_LEVEL_INFO, log_data);

  digitalWrite(ledPin,LOW);
  delayMicroseconds(samplingTime);
  int voMeasured = analogRead(measurePin);
  delayMicroseconds(deltaTime);
  digitalWrite(ledPin,HIGH);

  ext_snprintf_P(log_data, sizeof(log_data), "Measured ADC Value is %d", voMeasured);
  AddLog(LOG_LEVEL_INFO, log_data);

  float calcVoltage = voMeasured*(50.0f/10240);
  dust_value = (float)(0.17f *calcVoltage - 0.1f);

  ext_snprintf_P(log_data, sizeof(log_data), "ADC Voltage: %_f, Dust Density: %_f", &calcVoltage, &dust_value);
  AddLog(LOG_LEVEL_INFO, log_data);

  if ( dust_value < 0){
    dust_value = 0.00;
  }
// dust_value = 0.01f;

}

void DustSensorInit() {
    pinMode(Pin(GPIO_GP2Y1010AU0F), OUTPUT);
}

bool Xsns88(uint8_t function) {
    bool result = false;
    switch(function){
        case FUNC_INIT:
            DustSensorInit();
            break;
        case FUNC_EVERY_SECOND:
           DustSensorEverySecond();
           break;
        case FUNC_JSON_APPEND:
            DustSensorShow(1);
            break;    
    #ifdef USE_WEBSERVER
        case FUNC_WEB_SENSOR:
            DustSensorShow(0);
            break;
    #endif  // USE_WEBSERVER           
    }
    return result;
}
#endif