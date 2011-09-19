#ifndef _TEST_APP
#define _TEST_APP
#include "ofMain.h"

/************************/
#include "ofxOsc.h"
/************************/

#define NUM 1000
/************************/
// スクリーン 1 のポートは12345
#define RECV_PORT 12345 
// HOSTは状況に合わせて変更, IPアドレスは文字列で指定すること
#define HOST "localhost"
// スクリーン1はポート3333へデータを送信
#define SEND_PORT 3333
/************************/

#define NUM_MSG_STRINGS 25
class testApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    
    void keyPressed  (int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    
    /**********************************/
    // コントローラ(OSC)から送られたKeyの処理
    void keyPressedOSC(int key);
    /**********************************/
    
    float angle; //現在の角度
    float radius; //全体の半径
    ofPoint pos[NUM]; //全ての円の座標を格納する配列
    ofPoint speed; //x, yの座標ごとの角速度


    /**********************************/
    ofxOscReceiver	receiver;
    ofxOscSender    sender;
    /**********************************/

    int				current_msg_string;
    string		msg_strings[NUM_MSG_STRINGS];
    float			timers[NUM_MSG_STRINGS];

    int				mouseX, mouseY;
    string			mouseButtonState;
};

#endif
