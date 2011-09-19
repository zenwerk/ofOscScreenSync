#include "testApp.h"

#include "beatDetect.h"

// 送信先ホスト
#define HOST "localhost"

#define PORT1 12345
#define PORT2 23456

// 自身がメッセージを受取るときに使用するポート
#define RECEIVE_PORT1 3333

int testApp_buffer_size = 1024, testApp_fft_size = 512;

int counter = 0;

bool bSendFFTValue = false;
bool bKickDetected = false;
bool bSnareDetected = false;
bool bHihatDetected = false;

// スクリーン描画制御フラグ
bool bScreen1 = false;
bool bScreen2 = false;

void testApp::setup(){
	// フレームレートを60Hzにセット
	ofSetFrameRate(60);
	
	// オーディオインプットを指定 この値は固定すること
    // Output->0 Input->mono  sampleRate->44100  bufSize->1024
	ofSoundStreamSetup(0, 1, this, 44100, testApp_buffer_size, 4);

    // 二つのクライアントに対して2つのsender
	sender.setup( HOST, PORT1 );
    sender2.setup(HOST, PORT2);

    // 二つのクライアントから送られてくるフラグ受取るreceiver
    receiver.setup(RECEIVE_PORT1);

    //fbook.loadFont("frabk.ttf", 32, true, false);
	ofBackground(20,20,20);
    printf("testApp setup() OK!\n");
}

//------------------------------------------------------------------------
void testApp::audioReceived(float* input, int bufferSize, int nChannels) {
    bd.audioReceived(input, bufferSize);
}


//------------------------------------------------------------------------
void testApp::keyPressed(int key){
    if(key == '\\'){
        bSendFFTValue = !bSendFFTValue;
    }
    // 押されたkeyをOSCで送信
    ofxOscMessage m;
    m.setAddress( "/testApp/keyPressed" );
    m.addIntArg( key );
    sender.sendMessage( m );
    sender2.sendMessage(m);
    pressedKey = key;
}

//------------------------------------------------------------------------
void testApp::update() {
    bKickDetected = false;
    bSnareDetected = false;
    bHihatDetected = false;

    bd.updateFFT();
    if(bSendFFTValue){
        ofxOscMessage fftVal;
        fftVal.setAddress("/testApp/fftVal");
        for(int i=0; i<testApp_fft_size; i++){
            fftVal.addFloatArg(bd.magnitude[i]);
        }
        sender.sendMessage(fftVal);
        sender2.sendMessage(fftVal);
    }


    // ビート検出を行い結果をOSCで送信
    ofxOscMessage oscbd;
    oscbd.setAddress("/testApp/BeatDetect");
    // ドラム音の検出
    if(bd.isBeatRange(0,2,2)){
        oscbd.addStringArg("KickDetected");
        bKickDetected = true;
    }    
    // スネア音の検出
    if(bd.isBeatRange(12,18,4)){
        oscbd.addStringArg("SnareDetected");
        bSnareDetected = true;
    }
    // ハイハット音の検出
    if(bd.isBeatRange(27,31,3)){
        oscbd.addStringArg("HihatDetected");
        bHihatDetected = true;
    }
    oscbd.addStringArg("end");
    sender.sendMessage(oscbd);
    sender2.sendMessage(oscbd);


	// check for waiting messages
    // スクリーン描画フラグを設定
	while( receiver.hasWaitingMessages() ) {
		// get the next message
		ofxOscMessage m;
		receiver.getNextMessage( &m );
        if(m.getAddress() == "/testApp/DrawComplete"){
			for ( int i=0; i<m.getNumArgs(); i++ ) {
                //if(m.getArgAsString( i ) == "end") break;
                if(m.getArgAsString(i) == "Screen1")
                    bScreen1 = true;
                else if(m.getArgAsString(i) == "Screen2")
                    bScreen2 = true;
                else;
            }
        }
    }

    // 二つのクライアントから描画完了フラグを受け取ったら
    // 次のフレームの描画を指示する
    if(bScreen1 && bScreen2){
        ofxOscMessage m;
        m.setAddress( "/testApp/bDrawFlag" );
        m.addStringArg( "DrawScreen" );
        sender.sendMessage( m );
        sender2.sendMessage(m);
        bScreen1 = bScreen2 = false;
        counter++;
        cout << "DrawScreen!!: " << counter << endl;
    }
}


//------------------------------------------------------------------------
void testApp::draw() {
	// display instructions
	string buf;
	buf = "sending osc messages to" + string( HOST ) + ofToString( PORT1 );
	ofDrawBitmapString( buf, 10, 20 );
	ofDrawBitmapString( "Sending the pressed Key to the Client", 10, 50 );
	//ofDrawBitmapString( "click to send osc message [/mouse/button <button> <\"up\"|\"down\">]", 10, 65 );
    if(bSendFFTValue) ofDrawBitmapString( "Sending the FFT Value!!", 10, 80 );
    ofDrawBitmapString("Pressed Key = " + ofToString(pressedKey), 10, 100);
    if(bKickDetected) ofDrawBitmapString("Kick Detected!!", 10, 120);
    if(bSnareDetected) ofDrawBitmapString("Snare Detected!!", 10, 140);
    if(bHihatDetected) ofDrawBitmapString("Hihat Detected!!", 10, 160);
}

void testApp::keyReleased(int key){

}

void testApp::mouseMoved(int x, int y){

}

void testApp::mouseDragged(int x, int y, int button){

}

void testApp::mousePressed(int x, int y, int button){

}

void testApp::mouseReleased(int x, int y, int button){

}

void testApp::windowResized(int w, int h){

}
