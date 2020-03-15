//代码还不完善，请酌情使用
//by xqt
//控制脚 12,13,14,16,
#define BLINKER_PRINT Serial
#define BLINKER_WIFI
#define BLINKER_MIOT_LIGHT
#include <Blinker.h>
/***************此处要修改******************/
char auth[] = "**********";    /****秘钥****/
char ssid[] = "**********";    /**WiFi名称**/
char pswd[] = "**********";    /**WiFi密码**/
/***************以上要修改**************/
// 新建组件对象
// 以下此处的组件名称要和app端设置的匹配
BlinkerButton Button1("btn1");
BlinkerNumber Number1("num1");
BlinkerText   Text1("tex-1");//R 0~255
BlinkerText   Text2("tex-2");//G 0~255
BlinkerText   Text3("tex-3");//B 0~255
BlinkerRGB    RGB1("RGBKey"); 

int counter = 0;  //无关指令计数
int LED_R=0,LED_G=0,LED_B=0,LED_Bright=80;// RGB和亮度
int LR=0,LG=0,LB=0,BL=0;
bool LED_Flag = false;

// 按下按键即会执行该函数
void button1_callback(const String & state) {
    if (state == BLINKER_CMD_ON) {
        BlinkerMIOT.powerState("on");
        BlinkerMIOT.print();
        Button1.print("on");     //反馈状态给APP
        LED_Flag = true;
    }
    else if (state == BLINKER_CMD_OFF) {
        BlinkerMIOT.powerState("off");
        BlinkerMIOT.print();
        Button1.print("off");
        LED_Flag = false;
    }
    BLINKER_LOG("get button state: ", state);  
    if(LED_Flag) SET_RGB(255,255,255,LED_Bright);
    else SET_RGB(0,0,0,0);
}

// 如果未绑定的组件被触发，则会执行其中内容
void dataRead(const String & data)
{
    BLINKER_LOG("Blinker readString: ", data);
    counter++;
    Number1.print(counter);                 //无关指令计数
}
void SET_RGB(int R,int G,int B,int bright)  //亮度和rgb值算法
{  
    R = R*bright/100;
    G = G*bright/100;
    B = B*bright/100;
    
    LED_R = map(R,0,255,0,1024);
    LED_G = map(G,0,255,0,1024);
    LED_B = map(B,0,255,0,1024);
    Serial.printf("R=%d G=%d B=%d bright=%d\r\n",LED_R,LED_G,LED_B,bright);
    analogWrite(12,1024-LED_R);  
    analogWrite(14,1024-LED_G); 
    analogWrite(16,1024-LED_B); 
}
//APP RGB颜色设置回调
void rgb1_callback(uint8_t r_value, uint8_t g_value, 
                    uint8_t b_value, uint8_t bright_value)
{
    

    BLINKER_LOG("R value: ", r_value);
    BLINKER_LOG("G value: ", g_value);
    BLINKER_LOG("B value: ", b_value);
    BLINKER_LOG("Rrightness value: ", bright_value);
    LED_Bright = map(bright_value,0,255,0,100);
    SET_RGB(r_value,g_value,b_value,LED_Bright);
}
//小爱电源类回调
void miotPowerState(const String & state)
{
    BLINKER_LOG("need set power state: ", state);

    if (state == BLINKER_CMD_ON) {
        
        //digitalWrite(13, LOW);
        
        SET_RGB(255,255,255,LED_Bright);        
        BlinkerMIOT.powerState("on");
        BlinkerMIOT.print();
        Button1.print("on");
    }
    else if (state == BLINKER_CMD_OFF) {
        
        //digitalWrite(13, HIGH);
        
        SET_RGB(0,0,0,0);
        BlinkerMIOT.powerState("off");
        BlinkerMIOT.print();
        Button1.print("off");
    }
}
//小爱设置颜色回调
void miotColor(int32_t color)
{
    BLINKER_LOG("need set color: ", color);
    int colorR,colorG,colorB;
    colorR = color >> 16 & 0xFF;
    colorG = color >>  8 & 0xFF;
    colorB = color       & 0xFF;
      LR=colorR;
      LG=colorG;
      LB=colorB;
    BLINKER_LOG("colorR: ", colorR, ", colorG: ", colorG, ", colorB: ", colorB);
    SET_RGB(colorR,colorG,colorB,LED_Bright);
    BlinkerMIOT.color(color);//反馈小爱控制状态
    BlinkerMIOT.print();
    RGB1.print(colorR,colorG,colorB,LED_Bright);
    Text1.print("R=",colorR);
    Text2.print("G=",colorG);
    Text3.print("B=",colorB);
}
//小爱调亮度回调
void miotBright(const String & bright)
{
    BLINKER_LOG("need set brightness: ", bright);
    int colorW = bright.toInt();
    BLINKER_LOG("now set brightness: ", colorW);
    LED_Bright = colorW;
    SET_RGB(LED_R,LED_G,LED_B,LED_Bright);
    Serial.printf("light set...%d",colorW);
    BlinkerMIOT.brightness(colorW);//反馈小爱控制状态
    BlinkerMIOT.print(); 
    BLINKER_LOG("cR: ", LED_R, ", cG: ", LED_G, ", cB: ",LED_B);
        BL = map(LED_Bright,0,100,0,255);
    RGB1.print(LR,LG,LB,BL);
    Text1.print("R=",LR);
    Text2.print("G=",LG);
    Text3.print("B=",LB);
}
void setup() {
    // 初始化串口
    Serial.begin(115200);
    #if defined(BLINKER_PRINT)
        BLINKER_DEBUG.stream(BLINKER_PRINT);
    #endif
    // 初始化有LED的IO
    pinMode(13, OUTPUT);
    pinMode(12, OUTPUT);//红
    pinMode(14, OUTPUT);//绿色
    pinMode(16, OUTPUT);//蓝色
    SET_RGB(0,0,0,0);
    digitalWrite(13, HIGH);
  // 初始化blinker
    Blinker.begin(auth, ssid, pswd);             //连接WiFi和设备上线
    Blinker.attachData(dataRead);                //如果没有被绑定的组件被触发就执行这个回调
    Button1.attach(button1_callback);            //Button1这个组件被触发就执行这个回调
    RGB1.attach(rgb1_callback);                  //注册调节颜色的回调函数
    BlinkerMIOT.attachPowerState(miotPowerState);
    BlinkerMIOT.attachColor(miotColor);          //小爱调节颜色
    BlinkerMIOT.attachBrightness(miotBright);    //小爱调节RGB亮度
}

void loop() {
    Blinker.run();
}
