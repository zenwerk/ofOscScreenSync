#include "testApp.h"

/**********************************/
int oscKey = 0;
float oscFFTval[512];
// ビート検出フラグ
bool bIsKick = false;
bool bIsSnare = false;
bool bIsHihat = false;
// 画面同期フラグ
bool bStartFlag = false;
bool bDrawFlag = false;
/**********************************/



//--------------------------------------------------------------
void testApp::setup(){
    //画面の基本設定
    ofBackground(0, 0, 0);
    ofSetFrameRate(30);
    ofEnableAlphaBlending();
    ofSetCircleResolution(128);
    
    //円軌道の半径を設定
    radius = ofGetHeight()/4.0;
    //x, y軸の初期の角速度を設定
    speed.x = speed.y = 1.0;

    // for OSC
	current_msg_string = 0;
	mouseX = 0;
	mouseY = 0;
	mouseButtonState = "";

    /******************************************************************/
	// listen on the given port
	cout << "listening for osc messages on port " << RECV_PORT << "\n";
    // OSC送受信器のセットアップ
	receiver.setup( RECV_PORT );
	sender.setup( HOST, SEND_PORT );
    /******************************************************************/
}

//--------------------------------------------------------------
void testApp::update(){
    /******************************************************************/
    // ビート検出のフラグをオフに
    bIsKick = false;
    bIsSnare = false;
    bIsHihat = false;
    /******************************************************************/
	// check for waiting messages
    // OSCコントローラから送られてきたデータを解析する
	while( receiver.hasWaitingMessages() ) {
		// get the next message
		ofxOscMessage m;
		receiver.getNextMessage( &m );

		// keyPressed 押されたkey を受信
		if ( m.getAddress() == "/testApp/keyPressed" ) {
			// both the arguments are int32's
			oscKey = m.getArgAsInt32( 0 );
            keyPressedOSC(oscKey);
		}
        // fftVal 入力音声のfftの値を受信
        else if(m.getAddress() == "/testApp/fftVal"){
            for(int i=0; i<512; i++){
                oscFFTval[i] = m.getArgAsFloat(i);
            }
        }
        // BeatDetect ビート検出の結果を受信
        else if(m.getAddress() == "/testApp/BeatDetect"){
			for ( int i=0; i<m.getNumArgs(); i++ ) {
                //if(m.getArgAsString( i ) == "end") break;
                if(m.getArgAsString(i) == "KickDetected")
                    bIsKick = true;
                else if(m.getArgAsString(i) == "SnareDetected")
                    bIsSnare = true;
                else if(m.getArgAsString(i) == "HihatDetected")
                    bIsHihat = true;
            }
        }
        // 次のフレームを描画するフラグを真にする
        else if(m.getAddress() == "/testApp/bDrawFlag"){
            if(m.getArgAsString(0) == "DrawScreen")
                bDrawFlag = true;
        }
        // その他のOSCメッセージを処理する
		// check for mouse button message
		//else if ( m.getAddress() == "/mouse/button" ) {
		//	// the single argument is a string
		//	mouseButtonState = m.getArgAsString( 0 ) ;
		//}
		//else {
		//	// unrecognized message: display on the bottom of the screen
        //    // 未認識のOSCメッセージに対する処理
		//	string msg_string;
		//	msg_string = m.getAddress();
		//	msg_string += ": ";
		//	for ( int i=0; i<m.getNumArgs(); i++ ) {
		//		// 引数の型を取得する
		//		msg_string += m.getArgTypeName( i );
		//		msg_string += ":";

		//		// display the argument - make sure we get the right type
		//		if( m.getArgType( i ) == OFXOSC_TYPE_INT32 )
		//			msg_string += ofToString( m.getArgAsInt32( i ) );
		//		else if( m.getArgType( i ) == OFXOSC_TYPE_FLOAT )
		//			msg_string += ofToString( m.getArgAsFloat( i ) );
		//		else if( m.getArgType( i ) == OFXOSC_TYPE_STRING )
		//			msg_string += m.getArgAsString( i );
		//		else
		//			msg_string += "unknown";
		//	}
		//	// add to the list of strings to display
		//	msg_strings[current_msg_string] = msg_string;
		//	timers[current_msg_string] = ofGetElapsedTimef() + 5.0f;
		//	current_msg_string = ( current_msg_string + 1 ) % NUM_MSG_STRINGS;
		//	// clear the next line
		//	msg_strings[current_msg_string] = "";
		//}

	}
    /******************************************************************/

    /********************************************/
    if(bStartFlag){    // 一番最初のスタートフラグ
        if(bDrawFlag){ // フレーム同期フラグ
    /********************************************/
            //角度を更新
            angle+=100.0/NUM;
            //全ての円の座標を計算
            for (int i=0; i<NUM; i++) {
                //x座標とy座標で角速度を変化させている
                pos[i].x = sin(angle*i/NUM*speed.x)*radius;
                pos[i].y = cos(angle*i/NUM*speed.y)*radius;
            }
            /*******************************************************/
            // updateしたらその旨を通知
            ofxOscMessage m;
            m.setAddress("/testApp/DrawComplete");
            m.addStringArg( "Screen1" ); // <- スクリーン1は"Screen1"というフラグ
                                         //    スクリーン2のときは"Screen2"をsendMessageすること
            sender.sendMessage(m);
            bDrawFlag = false;
            /*******************************************************/
        }
    }



}

//--------------------------------------------------------------
void testApp::draw(){
    /***************/
    if(bStartFlag){
    /***************/
        //画面の中心を原点(0,0)にする
        ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
        //右に半画面分移動するofTranslate(ofGetWidth()/2, 0)
        ofTranslate(ofGetWidth()/2, 0);
        //色の設定
        ofSetColor(31, 127, 255, 127);
        ofNoFill();
        //全ての座標の円を描く
        for (int i=0; i<NUM; i++) {
            ofCircle(pos[i].x, pos[i].y, i*radius/NUM);
        }
        //cout << "send DrawComplete 1" << endl;

    }

}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

/**************************************************************/
// OSCで受信したキー入力の処理
// 基本的にkeyPressedと同じように書けばOK
void testApp::keyPressedOSC(int key){
    if(key == '1'){
        bStartFlag = true;
        bDrawFlag = true;
    }
}
/**************************************************************/

//--------------------------------------------------------------
void testApp::keyReleased(int key){
    //ofSetFullscreen(true);  
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
    //マウスをドラッグすると、x座標とy座標の角速度を別々に変更できる
    //speed.x = x * 2.0 / ofGetWidth();
    //speed.y = y * 2.0 / ofGetHeight();
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

