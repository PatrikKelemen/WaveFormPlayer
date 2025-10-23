#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#define OLC_SOUNDWAVE
#include "olcSoundWaveEngine.h"




class Example004 : public olc::PixelGameEngine
{
public:
	Example004()
	{
		sAppName = "Testing Custom Wave Gen";
	}

protected: // Sound Specific
	olc::sound::WaveEngine engine;

	olc::sound::Wave custom_wave;
	olc::sound::Wave custom_wave2;

	olc::sound::Wave tempWave;
	olc::sound::Wave wavelist[100];
	int counter =0;
	std::map<float, olc::sound::Wave> wavemap;
	
	float customWavePoints[100];  // Store Y values for each X position
    bool isDrawingWave = false;   // Track if mouse is held down
    int lastDrawX = -1;  
	 int resetWaveType = 0;	
		
	struct KeyButton {
        olc::Key key;
        std::string label;
        int waveformIndex;
        olc::vi2d position;
		bool white;
    };


	struct SongNote{
		int waveformIndex;
		float start;
	};

    std::vector<KeyButton> keyButtons;
	std::vector<SongNote> Song ;
	
	
	
	float inversePolyNomialWave(float exponent, int modifier, int wavePos){
	
		float y;
	
		if (wavePos<modifier/4 ){
			
			y = 0.4*( pow((wavePos - (0.0*modifier/4.0))* 1.0/float (modifier) * 4.0 ,exponent));
		}
		else if (wavePos<modifier/2 ){
			//y= (0.4 - float((wavePos - (modifier/4.0)) * 2.0 )* 1.0/float (modifier)* 0.8)* exp(-6*t)  ; 
			
			y = 0.4*( pow((wavePos - (modifier/4.0))* 1.0/float (modifier) * 4.0 -1,exponent));
		}
		
		else if (wavePos<3*modifier/4 ){
			y = 0.4*(-1* pow((wavePos - (modifier/2.0))* 1.0/float (modifier) * 4.0 ,exponent));
		}
		
		else {
			y = 0.4*(-1* pow((wavePos - (3.0*modifier/4.0))* 1.0/float (modifier) * 4.0 -1,exponent));
			
		}
		
		
		return y;
	}
	
	float polyQuaterWave(int exponent, float modifier, float wavePos){
		float y;
		y = 0.5*(pow(wavePos* 1.0/float (modifier) ,exponent));
		
		return y;
	}
	
	float polyHalfWave(int exponent, float modifier, float wavePos){
		float y;
		
		if (wavePos<modifier/2.0 ){
			y = 0.5*(pow(wavePos* 2.0/float (modifier) ,exponent));
		}
		else {
			y = -0.5*(pow((wavePos - modifier/2.0) * 2.0/float (modifier) ,exponent)) + 0.5;
			
		}
		return y;
	}
	
	float polyNomialWave(float exponent, int modifier, int wavePos){
	
		float y;
		if (wavePos<modifier/4 ){
			y = 0.4*(-1* pow(wavePos* 1.0/float (modifier) * 4.0 -1,exponent)+1);
		}
		else if (wavePos<modifier/2 ){
			//y= (0.4 - float((wavePos - (modifier/4.0)) * 2.0 )* 1.0/float (modifier)* 0.8)* exp(-6*t)  ; 
			y = 0.4*(-1* pow((wavePos - (modifier/4.0))* 1.0/float (modifier) * 4.0 ,exponent)+1);
		}
		
		else if (wavePos<3*modifier/4 ){
			y = 0.4*( pow((wavePos - (modifier/2.0))* 1.0/float (modifier) * 4.0 -1,exponent)-1);
		}
		
		else {
			
			y = 0.4*( pow((wavePos - (3.0*modifier/4.0))* 1.0/float (modifier) * 4.0 ,exponent)-1);
		}
	
		return y;
	}
	
	float exponetialQuarterWave(float bendAmount, float wavePos, float modifier ){
		float y;
		y = (0.5/(bendAmount-1))*( exp ( log(bendAmount)*(wavePos* 1.0/float (modifier)))-1);
		return y;
	}
	
	float exponetialFullWave(float bendAmount, int wavePos, int modifier ){
		float y;
		
		if (wavePos<modifier/4 ){
			y = exponetialQuarterWave( bendAmount,wavePos, modifier/4.0 );
		}
		else if (wavePos<modifier/2 ){
			//y= (0.4 - float((wavePos - (modifier/4.0)) * 2.0 )* 1.0/float (modifier)* 0.8)* exp(-6*t)  ; 
			y= exponetialQuarterWave( bendAmount,-1*(wavePos- 2*(modifier/4.0)), modifier/4.0 );
		}
		
		else if (wavePos<3*modifier/4 ){
			y = -1* exponetialQuarterWave( bendAmount,(wavePos-2*(modifier/4.0)), modifier/4.0 );
		}
		
		else {
			
			y = -1* exponetialQuarterWave( bendAmount,-1*(wavePos- 4*(modifier/4.0)), modifier/4.0 );
		}
		
		return y;
	}
	
	float exponetialFullWaveMirrored(float bendAmount, int wavePos, int modifier ){ // same as full wave but inverse bend 
		float y;
		
		if (wavePos<modifier/4 ){
			y = -1.0* exponetialQuarterWave( bendAmount,-1*(wavePos - 1*(modifier/4.0)) , modifier/4.0 ) +0.5;
		}
		else if (wavePos<modifier/2 ){
			//y= (0.4 - float((wavePos - (modifier/4.0)) * 2.0 )* 1.0/float (modifier)* 0.8)* exp(-6*t)  ; 
			y= -1.0*exponetialQuarterWave( bendAmount,(wavePos- 1*(modifier/4.0)), modifier/4.0 ) +0.5;
		}
		
		else if (wavePos<3*modifier/4 ){
			y =  exponetialQuarterWave( bendAmount,-1*(wavePos-3*(modifier/4.0)), modifier/4.0 ) -0.5;
		}
		
		else {
			
			y =  exponetialQuarterWave( bendAmount,(wavePos- 3*(modifier/4.0)), modifier/4.0 ) -0.5;
		}
		
		return y;
	}
	
public:
	bool OnUserCreate() override
	{
		engine.InitialiseAudio(44100, 1);

		// Initialize custom wave points to zero
		for (int i = 0; i < 100; i++) {
			customWavePoints[i] = 0.0f;
		}

		custom_wave = olc::sound::Wave(1, sizeof(uint8_t), 44100, 44100);
		//Song.push_back({8,1});
		//*
		 //intro part 
		Song.push_back({8,1});
		Song.push_back({10,2});
		Song.push_back({11,3});
		Song.push_back({13,4});
		Song.push_back({11,5});
		Song.push_back({10,6});
		Song.push_back({8,7});
		Song.push_back({8,8});
		Song.push_back({7,9});
		Song.push_back({8,10});
		Song.push_back({10,11});
		
		Song.push_back({5,12});
		Song.push_back({7,12.3});
		Song.push_back({8,12.6});
		Song.push_back({10,13});
		
		
		Song.push_back({8,14});
		Song.push_back({7,14.3});
		Song.push_back({5,14.6});
		Song.push_back({7,15});
		
		//*/
		
		
		/* 
		//verses
		Song.push_back({8,1});
		Song.push_back({10,2});
		Song.push_back({11,3});
		Song.push_back({13,4});
		Song.push_back({11,5});
		Song.push_back({10,6});
		Song.push_back({8,7});
		Song.push_back({8,8});
		Song.push_back({7,9});
		Song.push_back({8,10});
		Song.push_back({10,11});
		Song.push_back({8,12});
		Song.push_back({7,13});
		
		//*/
		
		
		/* 
		//corus1
		Song.push_back({20,1});
		Song.push_back({15,1});
		Song.push_back({11,1});
		
		Song.push_back({18,2});
		Song.push_back({13,2});
		Song.push_back({10,2});
		
		Song.push_back({18,3});
		Song.push_back({13,3});
		Song.push_back({10,3});
		
		Song.push_back({11,4});
		Song.push_back({6,4}); // do not natrualize this time
		Song.push_back({3,4});
		
		Song.push_back({11,5});
		Song.push_back({6,5}); // do not natrualize this time
		Song.push_back({3,5});
		
		Song.push_back({13,5.5});
		Song.push_back({10,5.5}); 
		Song.push_back({6,5.5});
		
		Song.push_back({13,7});
		Song.push_back({10,7}); 
		Song.push_back({6,7});
		
		Song.push_back({15,8});
		Song.push_back({11,8}); 
		Song.push_back({8,8});
		
		Song.push_back({15,9});
		Song.push_back({11,9}); 
		Song.push_back({8,9});
		
		Song.push_back({13,10});
		Song.push_back({8,10});
		
		Song.push_back({11,11});
		Song.push_back({8,11});
		
		Song.push_back({15,11.5});
		Song.push_back({11,11.5}); 
		Song.push_back({8,11.5});
		
		//*/
		
		/* 
		//corus2
		Song.push_back({20,1});
		Song.push_back({15,1});
		Song.push_back({11,1});
		
		Song.push_back({18,2});
		Song.push_back({13,2});
		Song.push_back({10,2});
		
		Song.push_back({18,3});
		Song.push_back({13,3});
		Song.push_back({10,3});
		
		Song.push_back({11,4});
		Song.push_back({6,4}); // do not natrualize this time
		Song.push_back({3,4});
		
		Song.push_back({11,5});
		Song.push_back({6,5}); // do not natrualize this time
		Song.push_back({3,5});
		
		Song.push_back({13,5.5});
		Song.push_back({10,5.5}); 
		Song.push_back({6,5.5});
		
		Song.push_back({13,7});
		Song.push_back({10,7}); 
		Song.push_back({6,7});
		
		Song.push_back({15,8});
		Song.push_back({8,8}); 
		Song.push_back({4,8});
		
		Song.push_back({15,9});
		Song.push_back({8,9}); 
		Song.push_back({5,9});
		
		Song.push_back({13,10});
		Song.push_back({8,10});
		Song.push_back({5,10});
		
		Song.push_back({11,11});
		
		
		Song.push_back({15,11.5});
		Song.push_back({10,11.5}); 
		Song.push_back({7,11.5});
		
		//*/
		
		/*
			Song.push_back({11,0});
		Song.push_back({6,0}); // do not natrualize this time
		Song.push_back({3,0});
		
		Song.push_back({18,1});
		Song.push_back({13,1});
		Song.push_back({10,1});
		
		
		Song.push_back({11,2});
		Song.push_back({6,2}); // do not natrualize this time
		Song.push_back({3,2});
		
		
	
		
		
		
		Song.push_back({18,2.5});
		Song.push_back({13,2.5});
		Song.push_back({10,2.5});
		
		
			Song.push_back({20,3});
		Song.push_back({15,3});
		Song.push_back({11,3});
		//*/ 
		
		/*
		Song.push_back({10,0});
		Song.push_back({19,0});
		Song.push_back({22,0});
		Song.push_back({26,0});
		
		Song.push_back({10,2});
		Song.push_back({19,2});
		Song.push_back({22,2});
		Song.push_back({26,2});
		
		
		Song.push_back({10,4.5});
		Song.push_back({19,4.5});
		Song.push_back({22,4.5});
		Song.push_back({26,4.5});
		
		Song.push_back({10,5});
		Song.push_back({19,5});
		Song.push_back({22,5});
		Song.push_back({26,5});
		
		
		Song.push_back({9,6});
		Song.push_back({19,6});
		Song.push_back({21,6});
		Song.push_back({26,6});
		
		Song.push_back({9,8});
		Song.push_back({19,8});
		Song.push_back({21,8});
		Song.push_back({26,8});
		
		Song.push_back({9,10.5});
		Song.push_back({19,10.5});
		Song.push_back({21,10.5});
		Song.push_back({26,10.5});
		
		Song.push_back({9,11});
		Song.push_back({19,11});
		Song.push_back({21,11});
		Song.push_back({26,11});
		
		Song.push_back({7,12});
		Song.push_back({17,12});
		Song.push_back({19,12});
		Song.push_back({24,12});
		
		Song.push_back({7,14});
		Song.push_back({17,14});
		Song.push_back({19,14});
		Song.push_back({24,14});
		
		Song.push_back({7,16.5});
		Song.push_back({17,16.5});
		Song.push_back({19,16.5});
		Song.push_back({24,16.5});
		
		Song.push_back({7,17});
		Song.push_back({17,17});
		Song.push_back({19,17});
		Song.push_back({24,17});
		
		Song.push_back({5,18});
		Song.push_back({15,18});
		Song.push_back({17,18});
		Song.push_back({22,18});
		
		
		
		Song.push_back({5,20});
		Song.push_back({15,20});
		Song.push_back({17,20});
		Song.push_back({22,20});
		
		Song.push_back({5,22.5});
		Song.push_back({15,22.5});
		Song.push_back({17,22.5});
		Song.push_back({22,22.5});
		
		Song.push_back({5,23});
		Song.push_back({15,23});
		Song.push_back({17,23});
		Song.push_back({22,23});
		//*/
		
		
		/*
		Song.push_back({11,1});
		Song.push_back({14,2.5});
		Song.push_back({7,3});
		
		Song.push_back({11,4});
		Song.push_back({14,5.5});
		Song.push_back({7,6});
		
		Song.push_back({11,7});
		Song.push_back({14,8.5});
		Song.push_back({7,9});
		
		Song.push_back({11,10});
		Song.push_back({14,11.5});
		Song.push_back({7,12});
		
		
		Song.push_back({10,13});
		Song.push_back({14,14.5});
		Song.push_back({7,15});
		
		Song.push_back({10,16});
		Song.push_back({14,17.5});
		Song.push_back({7,18});
		
		Song.push_back({10,19});
		Song.push_back({14,20.5});
		Song.push_back({7,21});
		
		Song.push_back({10,22});
		Song.push_back({14,23.5});
		Song.push_back({7,24});
		
		
		Song.push_back({9,25});
		Song.push_back({12,26.5});
		Song.push_back({5,27});
		
		Song.push_back({9,28});
		Song.push_back({12,29.5});
		Song.push_back({5,30});
		
		Song.push_back({9,31});
		Song.push_back({12,32.5});
		Song.push_back({5,33});
		
		Song.push_back({9,34});
		Song.push_back({12,35.5});
		Song.push_back({5,36});
		
		Song.push_back({8,37});
		Song.push_back({12,38.5});
		Song.push_back({5,39});
		
		Song.push_back({8,40});
		Song.push_back({12,41.5});
		Song.push_back({5,42});
		
		Song.push_back({8,43});
		Song.push_back({12,44.5});
		Song.push_back({5,45});
		
		Song.push_back({8,46});
		Song.push_back({12,47.5});
		Song.push_back({5,48});
		
		
		Song.push_back({11,49});
		Song.push_back({14,50.5});
		Song.push_back({7,51});
		//*/
		
		
		for (float freq = 110.0/1.0;freq <=1760.0 * (0.5);freq*=exp(log(2.0)/12.0)){  //change 12 to 31 to get 31 notes per octave and vice versa remember to update the drawing function one as well until made better
			
			wavelist[counter]= olc::sound::Wave(1, sizeof(uint8_t), 44100, 44100);
			for (size_t i = 0; i < 44100; i++){
				double dt = 1.0 / 44100.0;
				double t = double(i) * dt;
				float y =0;
				 //y= float(0.5 * sin(2* freq * 3.14159 * t) )*exp(-6*t);
				
				  //y += float(0.3 * sin(1.0226*2 * freq * 3.14159 * t) );
				 // y += float(0.2 * sin(1.005*2 * freq * 3.14159 * t) );
				 // y += float(0.2 * sin(1.01*2 * freq * 3.14159 * t) );
				 //y += float(0.2 * sin((1.0/1.025)*2 * freq * 3.14159 * t) );
				 // y += float(0.2 * sin((1.0/1.005)*2 * freq * 3.14159 * t) );
				 // y += float(0.2 * sin((1.0/1.01)*2 * freq * 3.14159 * t) );
				//float y = float(0.3 * sin(1.2*2 * freq * 3.14159 * t) );
				
				
   
				
				//y*= 0.8*exp(-4*t);
				//y += float(0.2 *sin(1 * freq * 3.14159 * t) * exp(-0.01 * freq*t) );
				//y += float(0.1 *sin(0.05 * freq * 3.14159 * t) * exp(-0.01 * freq*t) );
				//y += float(0.02 *sin(0.5 * freq * 3.14159 * t) * exp(-0.008 * freq*t) );
				//y = y*y*y;
				//y += float(0.3 * sin(0.5 * freq * 3.14159 * t) * exp(-6 *t));
				//y += float(0.2 *sin(1 * freq * 3.14159 * t) * exp(-6*t) );
				//y *=  1+16*t*exp(-12*t);
				//y *=  1+16*t*exp(-12*t);
				//y *= 1- ((double) rand() / (RAND_MAX))/(4*exp(-3*(t-1))) ;
				
				//*
				//square wave test 
				if(sin(1 * freq * 3.14159 * t) >0){
					y+=0.4* exp(-6 *t);
				}
				else {
					y+=-0.4* exp(-6 *t);
				}
				//*/
				int modifier = int(44100/(0.5*freq));
				
				int wavePos = i%modifier;
				//std::cout << wavePos << "\n"; 
				/* //triangle wave
				if (wavePos<modifier/4 ){
					y+= (float(wavePos * 4 )* 1.0/float (modifier) * 0.4)* exp(-6*t)  ; 
					//std::cout<< std::to_string(float(wavePos * 4 )* (1/modifier) * 0.5) << "\n";
					//std::cout << std::to_string(y) << " : " << std::to_string(freq) << "\n"; 
				}
				
				
				else if (wavePos<3*modifier/4 ){
				y+= (0.4 - float((wavePos - (modifier/4.0)) * 2.0 )* 1.0/float (modifier)* 0.8)* exp(-6*t)  ;
				}
				else {
					y += (-0.4 + float((wavePos - (3*modifier/4.0)) * 4.0 )* 1.0/float (modifier)* 0.4 )* exp(-6*t) ;
					
				}//*/
				
				y/=3.0;
				
				/*
				// it sounds like a piano so no trumpet 

				// Base fundamental and harmonics with odd harmonics emphasized
				y = float(0.4 * sin(1.0 * freq * 2 * 3.14159 * t) * exp(-0.8 * t));      // Fundamental
				y += float(0.25 * sin(2.0 * freq * 2 * 3.14159 * t) * exp(-1.2 * t));    // 2nd harmonic
				y += float(0.3 * sin(3.0 * freq * 2 * 3.14159 * t) * exp(-0.9 * t));     // 3rd harmonic (strong odd)
				y += float(0.15 * sin(4.0 * freq * 2 * 3.14159 * t) * exp(-1.5 * t));    // 4th harmonic
				y += float(0.2 * sin(5.0 * freq * 2 * 3.14159 * t) * exp(-1.0 * t));     // 5th harmonic (strong odd)
				y += float(0.1 * sin(6.0 * freq * 2 * 3.14159 * t) * exp(-1.8 * t));     // 6th harmonic
				y += float(0.12 * sin(7.0 * freq * 2 * 3.14159 * t) * exp(-1.3 * t));    // 7th harmonic

				// Sharp attack envelope - trumpets have a quick, punchy start
				y *= (1.0 + 8.0 * t * exp(-25.0 * t));

				// Slight vibrato for realism
				y *= (1.0 + 0.03 * sin(5.5 * 3.14159 * t));
				y *= 0.4* exp(-2 *t); //making it decay faster and lowering it to prevent clipping 
				// Overall amplitude envelope with moderate decay
				//y *= exp(-1.2 * t);
				//*/
				
				/*
				
				y = float(0.35 * sin(1.0 * freq * 3.14159 * t) * exp(-3 * t));  // Fundamental
				y += float(0.25 * sin(2.0 * freq * 3.14159 * t) * exp(-4 * t)); // 2nd harmonic - body
				y += float(0.2 * sin(3.0 * freq * 3.14159 * t) * exp(-5 * t));  // 3rd harmonic - brightness
				y += float(0.15 * sin(4.0 * freq * 3.14159 * t) * exp(-6 * t)); // 4th harmonic - brass
				y += float(0.1 * sin(5.0 * freq * 3.14159 * t) * exp(-7 * t));  // 5th harmonic - edge
				y += float(0.05 * sin(6.0 * freq * 3.14159 * t) * exp(-8 * t)); // 6th harmonic - shimmer
				y *= (1 + 3 * t * exp(-12 * t)); // Attack envelope - gives it that "punch"
				
				y*=0.5;
				
			    //*/
				
				/*
				//  MODIFIED VERSION 
				y = float(0.25 * sin(1.0 * freq * 3.14159 * t) * exp(-3 * t));  // Fundamental
				y += float(0.15 * sin(2.0 * freq * 3.14159 * t) * exp(-4 * t)); // 2nd harmonic - body
				y += float(0.1 * sin(3.0 * freq * 3.14159 * t) * exp(-5 * t));  // 3rd harmonic - brightness
				y += float(0.05 * sin(4.0 * freq * 3.14159 * t) * exp(-6 * t)); // 4th harmonic - brass
				y += float(0.05 * sin(5.0 * freq * 3.14159 * t) * exp(-7 * t));  // 5th harmonic - edge
				y += float(0.02 * sin(6.0 * freq * 3.14159 * t) * exp(-8 * t)); // 6th harmonic - shimmer
				y *= (1 + 3 * t * exp(-12 * t)); // Attack envelope - gives it that "punch"
			    //*/
				
				/*
				// Trumpet-like waveform it sounds like a piano so no trumpet 
				y = float(0.35 * sin(1.0 * freq * 3.14159 * t) * exp(-3 * t));  // Fundamental
				y += float(0.25 * sin(2.0 * freq * 3.14159 * t) * exp(-4 * t)); // 2nd harmonic - body
				y += float(0.2 * sin(3.0 * freq * 3.14159 * t) * exp(-5 * t));  // 3rd harmonic - brightness
				y += float(0.15 * sin(4.0 * freq * 3.14159 * t) * eexp(-6 * t)); // 4th harmonic - brass
				y += float(0.1 * sin(5.0 * freq * 3.14159 * t) * exp(-7 * t));  // 5th harmonic - edge
				y += float(0.05 * sin(6.0 * freq * 3.14159 * t) * exp(-8 * t)); // 6th harmonic - shimmer
				
			    //*/
				
				/*
				if (wavePos<modifier/4 ){
					y= (float(wavePos * 4 )* 1.0/float (modifier) * 0.5)* exp(-6*t)  ; 
					//std::cout<< std::to_string(float(wavePos * 4 )* (1/modifier) * 0.5) << "\n";
					//std::cout << std::to_string(y) << " : " << std::to_string(freq) << "\n"; 
				}
				
				
				else if (wavePos<3*modifier/4 ){
				y=   float(0.5* sin(2.0* 3.14159 * wavePos/float (modifier)) * exp(-6*t));
				}
				else {
					y = (-0.5 + float((wavePos - (3*modifier/4.0)) * 4.0 )* 1.0/float (modifier)* 0.5 )* exp(-6*t) ;
					
				}
				//*/
				
				/*
				if (wavePos<modifier/4 ){
					y= (float(wavePos * 4 )* 1.0/float (modifier) * 0.5)* exp(-6*t)  ; 
					y=   float(0.5* sin(2.0* 3.14159 * wavePos/float (modifier)) * exp(-6*t));
					//std::cout<< std::to_string(float(wavePos * 4 )* (1/modifier) * 0.5) << "\n";
					//std::cout << std::to_string(y) << " : " << std::to_string(freq) << "\n"; 
				}
				
				
				else if (wavePos<3*modifier/4 ){
				y= (0.4 - float((wavePos - (modifier/4.0)) * 2.0 )* 1.0/float (modifier)* 0.8)* exp(-6*t)  ;
				}
				else {
					y = (-0.5 + float((wavePos - (3*modifier/4.0)) * 4.0 )* 1.0/float (modifier)* 0.5 )* exp(-6*t) ;
					y=   float(0.5* sin(2.0* 3.14159 * wavePos/float (modifier)) * exp(-6*t));
					
				}
				//*/
				
				
				/*
				if (wavePos<modifier/4 ){
					y= 0.5* exp(-6*t);
				}
				
				
				else if (wavePos<3*modifier/4 ){
				y=   float(0.5* sin(2.0* 3.14159 * wavePos/float (modifier)) * exp(-6*t));
				}
				
				else {
					y = 0.5* exp(-6*t);
				}
				//*/
				
				/*
				if (wavePos<modifier/4 ){
					y= 0.5* exp(-6*t);
				}
				
				
				else if (wavePos<3*modifier/4 ){
				y = float(0.35 * sin(2.0* 3.14159 * wavePos/float (modifier)) * exp(-3 * t));  // Fundamental
				y += float(0.25 * sin(2.0 * 2.0* 3.14159 * wavePos/float (modifier)) * exp(-4 * t)); // 2nd harmonic - body
				y += float(0.2 * sin(3.0 * 2.0* 3.14159 * wavePos/float (modifier)) * exp(-5 * t));  // 3rd harmonic - brightness
				y += float(0.15 * sin(4.0 * 2.0* 3.14159 * wavePos/float (modifier)) * exp(-6 * t)); // 4th harmonic - brass
				y += float(0.1 * sin(5.0 * 2.0* 3.14159 * wavePos/float (modifier)) * exp(-7 * t));  // 5th harmonic - edge
				y += float(0.05 * sin(6.0 * 2.0* 3.14159 * wavePos/float (modifier)) * exp(-8 * t)); // 6th harmonic - shimmer
				}
				else {
					y = 0.5* exp(-6*t);
					
				}
				//*/
				
				
				
				/*
				if (wavePos<modifier/4 ){
					y= 0.5* exp(-6*t);
				}
				
				
				else if (wavePos<3*modifier/4 ){
				y = float(0.35 * sin(2.0* 3.14159 * wavePos/float (modifier)) * exp(-3 * t));  // Fundamental
				y += float(0.25 * sin(2.0 * 2.0* 3.14159 * wavePos/float (modifier)) * exp(-4 * t)); // 2nd harmonic - body
				y += float(0.2 * sin(3.0 * 2.0* 3.14159 * wavePos/float (modifier)) * exp(-5 * t));  // 3rd harmonic - brightness
				y += float(0.15 * sin(4.0 * 2.0* 3.14159 * wavePos/float (modifier)) * exp(-6 * t)); // 4th harmonic - brass
				y += float(0.1 * sin(5.0 * 2.0* 3.14159 * wavePos/float (modifier)) * exp(-7 * t));  // 5th harmonic - edge
				y += float(0.05 * sin(6.0 * 2.0* 3.14159 * wavePos/float (modifier)) * exp(-8 * t)); // 6th harmonic - shimmer
				}
				else {
					y = float(0.35 * sin(2.0* 3.14159 * wavePos/float (modifier)) * exp(-3 * t));  // Fundamental
				y += float(0.25 * sin(2.0 * 2.0* 3.14159 * wavePos/float (modifier)) * exp(-4 * t)); // 2nd harmonic - body
				y += float(0.2 * sin(3.0 * 2.0* 3.14159 * wavePos/float (modifier)) * exp(-5 * t));  // 3rd harmonic - brightness
				y += float(0.15 * sin(4.0 * 2.0* 3.14159 * wavePos/float (modifier)) * exp(-6 * t)); // 4th harmonic - brass
				y += float(0.1 * sin(5.0 * 2.0* 3.14159 * wavePos/float (modifier)) * exp(-7 * t));  // 5th harmonic - edge
				y += float(0.05 * sin(6.0 * 2.0* 3.14159 * wavePos/float (modifier)) * exp(-8 * t)); // 6th harmonic - shimmer
					
				}
				//*/
				
				/*
				if (wavePos<modifier/4 ){
					y= 0.5* exp(-6*t);
					
				}
				
				
				else if (wavePos<3*modifier/4 ){
				y=-0.5* exp(-6*t);
				}
				else {
					y = float(0.35 * sin(2.0* 3.14159 * wavePos/float (modifier)) * exp(-3 * t));  // Fundamental
				y += float(0.25 * sin(2.0 * 2.0* 3.14159 * wavePos/float (modifier)) * exp(-4 * t)); // 2nd harmonic - body
				y += float(0.2 * sin(3.0 * 2.0* 3.14159 * wavePos/float (modifier)) * exp(-5 * t));  // 3rd harmonic - brightness
				y += float(0.15 * sin(4.0 * 2.0* 3.14159 * wavePos/float (modifier)) * exp(-6 * t)); // 4th harmonic - brass
				y += float(0.1 * sin(5.0 * 2.0* 3.14159 * wavePos/float (modifier)) * exp(-7 * t));  // 5th harmonic - edge
				y += float(0.05 * sin(6.0 * 2.0* 3.14159 * wavePos/float (modifier)) * exp(-8 * t)); // 6th harmonic - shimmer
				y*=0.7;	
					
				}
				//*/
				
				
				
				/*
				if (wavePos<modifier/4 ){
					y = float(0.35 * sin(2.0* 3.14159 * wavePos/float (modifier)) * exp(-3 * t));  // Fundamental
				y += float(0.25 * sin(2.0 * 2.0* 3.14159 * wavePos/float (modifier)) * exp(-4 * t)); // 2nd harmonic - body
				y += float(0.2 * sin(3.0 * 2.0* 3.14159 * wavePos/float (modifier)) * exp(-5 * t));  // 3rd harmonic - brightness
				y += float(0.15 * sin(4.0 * 2.0* 3.14159 * wavePos/float (modifier)) * exp(-6 * t)); // 4th harmonic - brass
				y += float(0.1 * sin(5.0 * 2.0* 3.14159 * wavePos/float (modifier)) * exp(-7 * t));  // 5th harmonic - edge
				y += float(0.05 * sin(6.0 * 2.0* 3.14159 * wavePos/float (modifier)) * exp(-8 * t)); // 6th harmonic - shimmer	
					//std::cout<< std::to_string(float(wavePos * 4 )* (1/modifier) * 0.5) << "\n";
					//std::cout << std::to_string(y) << " : " << std::to_string(freq) << "\n"; 
				}
				
				
				else if (wavePos<3*modifier/4 ){
				y=-0.5* exp(-6*t);
				}
				else {
					y = float(0.35 * sin(2.0* 3.14159 * wavePos/float (modifier)) * exp(-3 * t));  // Fundamental
				y += float(0.25 * sin(2.0 * 2.0* 3.14159 * wavePos/float (modifier)) * exp(-4 * t)); // 2nd harmonic - body
				y += float(0.2 * sin(3.0 * 2.0* 3.14159 * wavePos/float (modifier)) * exp(-5 * t));  // 3rd harmonic - brightness
				y += float(0.15 * sin(4.0 * 2.0* 3.14159 * wavePos/float (modifier)) * exp(-6 * t)); // 4th harmonic - brass
				y += float(0.1 * sin(5.0 * 2.0* 3.14159 * wavePos/float (modifier)) * exp(-7 * t));  // 5th harmonic - edge
				y += float(0.05 * sin(6.0 * 2.0* 3.14159 * wavePos/float (modifier)) * exp(-8 * t)); // 6th harmonic - shimmer
					
					
				}
				//*/
				
				
				/*
				if (wavePos<modifier/4 ){
					y= 0.5* exp(-6*t);
				}
				else if (wavePos<3*modifier/4 ){
				y = float(0.35 * sin(2.0* 3.14159 * wavePos/float (modifier)) * exp(-3 * t));  // Fundamental
				y += float(0.25 * sin(2.0 * 2.0* 3.14159 * wavePos/float (modifier)) * exp(-4 * t)); // 2nd harmonic - body
				y += float(0.2 * sin(3.0 * 2.0* 3.14159 * wavePos/float (modifier)) * exp(-5 * t));  // 3rd harmonic - brightness
				y += float(0.15 * sin(4.0 * 2.0* 3.14159 * wavePos/float (modifier)) * exp(-6 * t)); // 4th harmonic - brass
				y += float(0.1 * sin(5.0 * 2.0* 3.14159 * wavePos/float (modifier)) * exp(-7 * t));  // 5th harmonic - edge
				y += float(0.05 * sin(6.0 * 2.0* 3.14159 * wavePos/float (modifier)) * exp(-8 * t)); // 6th harmonic - shimmer	
				}
				else {
					y = float(0.35 * sin(2.0* 3.14159 * wavePos/float (modifier)) * exp(-3 * t));  // Fundamental
				y += float(0.25 * sin(2.0 * 2.0* 3.14159 * wavePos/float (modifier)) * exp(-4 * t)); // 2nd harmonic - body
				y += float(0.2 * sin(3.0 * 2.0* 3.14159 * wavePos/float (modifier)) * exp(-5 * t));  // 3rd harmonic - brightness
				y += float(0.15 * sin(4.0 * 2.0* 3.14159 * wavePos/float (modifier)) * exp(-6 * t)); // 4th harmonic - brass
				y += float(0.1 * sin(5.0 * 2.0* 3.14159 * wavePos/float (modifier)) * exp(-7 * t));  // 5th harmonic - edge
				y += float(0.05 * sin(6.0 * 2.0* 3.14159 * wavePos/float (modifier)) * exp(-8 * t)); // 6th harmonic - shimmer	
					
				}
				//*/
				
				/*
				if (wavePos<modifier/4 ){
					y= 0.4* exp(-6*t);
				}
				else if (wavePos<modifier/2 ){
					y= (0.4 - float((wavePos - (modifier/4.0)) * 2.0 )* 1.0/float (modifier)* 0.8)* exp(-6*t)  ; 
					
				}
				
				else if (wavePos<3*modifier/4 ){
					y = -0.4* exp(-6*t);
				}
				
				else {
					
					y = (-0.4 + float((wavePos - (3*modifier/4.0)) * 4.0 )* 1.0/float (modifier)* 0.4 )* exp(-6*t) ;
					//y=0;
				}
				//*/
				 int exponent = 2;
				/*
				if (wavePos<modifier/4 ){
					y = 0.4*(-1* pow(wavePos* 1.0/float (modifier) * 4.0 -1,exponent)+1);
				}
				else if (wavePos<modifier/2 ){
					//y= (0.4 - float((wavePos - (modifier/4.0)) * 2.0 )* 1.0/float (modifier)* 0.8)* exp(-6*t)  ; 
					y = 0.4*(-1* pow((wavePos - (modifier/4.0))* 1.0/float (modifier) * 4.0 ,exponent)+1);
				}
				
				else if (wavePos<3*modifier/4 ){
					y = 0.4*( pow((wavePos - (modifier/2.0))* 1.0/float (modifier) * 4.0 -1,exponent)-1);
				}
				
				else {
					
					y = 0.4*( pow((wavePos - (3.0*modifier/4.0))* 1.0/float (modifier) * 4.0 ,exponent)-1);
				}
				//*/
				/*
				if (wavePos<modifier/4 ){
					y = 0.4*(-1* pow((wavePos - (0.0 * modifier/4.0))* 1.0/float (modifier) * 4.0 ,exponent)+1);
				}
				else if (wavePos<modifier/2 ){
					//y= (0.4 - float((wavePos - (modifier/4.0)) * 2.0 )* 1.0/float (modifier)* 0.8)* exp(-6*t)  ; 
					
					
					y = 0.4*(-1* pow((wavePos - (modifier/4.0))* 1.0/float (modifier) * 4.0 -1,exponent)+1);
				}
				
				else if (wavePos<3*modifier/4 ){
					y = 0.4*( pow((wavePos - (1.0*modifier/2.0))* 1.0/float (modifier) * 4.0 ,exponent)-1);
					
				}
				
				else {
					y = 0.4*( pow((wavePos - (3.0*modifier/4.0))* 1.0/float (modifier) * 4.0 -1,exponent)-1);
				}
				y= y* exp(-6*t);
				//*/
				/*	
				if (wavePos<modifier/4 ){
					
					y = 0.4*( pow((wavePos - (0.0*modifier/4.0))* 1.0/float (modifier) * 4.0 ,exponent));
				}
				else if (wavePos<modifier/2 ){
					//y= (0.4 - float((wavePos - (modifier/4.0)) * 2.0 )* 1.0/float (modifier)* 0.8)* exp(-6*t)  ; 
					
					y = 0.4*( pow((wavePos - (modifier/4.0))* 1.0/float (modifier) * 4.0 -1,exponent));
				}
				
				else if (wavePos<3*modifier/4 ){
					y = 0.4*(-1* pow((wavePos - (modifier/2.0))* 1.0/float (modifier) * 4.0 ,exponent));
				}
				
				else {
					y = 0.4*(-1* pow((wavePos - (3.0*modifier/4.0))* 1.0/float (modifier) * 4.0 -1,exponent));
					
				}
				//*/
				
				float bendAmount=200.0;
				//y=  polyNomialWave( exponent,  modifier, wavePos)*exp(-3*t);
				//y= 2*  inversePolyNomialWave(exponent ,  modifier, wavePos)*exp(-3*t);
			   
				//y = 1*exponetialFullWave(bendAmount,wavePos,modifier) *exp(-3*t);
				
				//y += 0.5*exponetialFullWaveMirrored(bendAmount,wavePos,modifier) *exp(-3*t);
				
				//y = polyHalfWave( exponent,  modifier, wavePos)*exp(-3*t);
				//y= 2.0*exponetialQuarterWave(bendAmount,wavePos,modifier) -0.5;
				/*
				y= 0.35* inversePolyNomialWave( exponent,  modifier, wavePos)*exp(-3*t);
				y+= 0.25 * inversePolyNomialWave( exponent,  int(44100/(2*freq)), i%(int(44100/(2*freq)))) *exp(-4*t);
				y+= 0.2 * inversePolyNomialWave( exponent,  int(44100/(3*freq)), i%(int(44100/(3*freq)))) *exp(-5*t);
				y+= 0.15 * inversePolyNomialWave( exponent,  int(44100/(4*freq)), i%(int(44100/(4*freq)))) *exp(-6*t);
				y+= 0.1 * inversePolyNomialWave( exponent,  int(44100/(5*freq)), i%(int(44100/(5*freq)))) *exp(-7*t) ;
				y+= 0.05 * inversePolyNomialWave( exponent,  int(44100/(6*freq)), i%(int(44100/(6*freq)))) *exp(-8*t);
				//y= y* exp(-6*t);
				//*/
				
				/*
				
				
				y = (0.5/(bendAmount-1))*( exp ( log(bendAmount)*(wavePos* 1.0/float (modifier)))-1);
				
				//*/
				//y*= ((double) rand() / (RAND_MAX))/4 + 0.75;
				//y*= 0.8*exp(-4*t);
				//y *= (1 + 3 * t * exp(-12 * t)); // Attack envelope - gives it that "punch"
				y *= std::min(1.0,exp(-80*(t-0.97))); //reset end position to zero
				y *= std::min(1.0,exp(300*(t-0.005))); //force start position to come in slower 
				
				wavelist[counter].file.data()[i] = y;
			}
			counter++;
		} 
		
		for (size_t i = 0; i < 44100; i++)
		{
			double dt = 1.0 / 44100.0;
			double t = double(i) * dt;
			
			float y = float(0.3 * sin(0.5 * 440.0 * 3.14159 * t) * exp(-0.015 * 440.0 *t));
			
			y += float(0.2 *sin(1.0 * 440.0 * 3.14159 * t) * exp(-0.015 * 440.0*t) );
			//y += float(0.05 *sin(4.0 * 440.0 * 3.14159 * t) * exp(-0.15 * 440.0*t) );
			//y += float(0.02 *sin(0.5 * 440.0 * 3.14159 * t) * exp(-0.008 * 440.0*t) );
			y += y*y*y;
			
			y *=  1+16*t*exp(-6*t);
			
			y *= std::min(1.0,exp(-100*(t-0.95))); //reset end position to zero
			
			custom_wave.file.data()[i] = y;
		}


	custom_wave2 = olc::sound::Wave(1, sizeof(uint8_t), 44100, 44100);

		for (size_t i = 0; i < 44100; i++)
		{
			double dt = 1.0 / 44100.0;
			double t = double(i) * dt;
			
			float y = float(0.3 * sin(1 * 880 * 3.14159 * t) * exp(-0.015 * 329.6276 *t));
			
			y += float(0.2 *sin(2.0 * 880* 3.14159 * t) * exp(-0.015 * 329.6276*t) );
			
			y += y*y*y;
			
			y *= 1 + 16*t*exp(-6*t);
			
			y *= std::min(1.0,exp(-100*(t-0.95)));
			
			custom_wave2.file.data()[i] = y;
		}

		
		keyButtons = {
            {olc::Key::E, "E", 3, {20, 60},true},
            {olc::Key::K4, "4", 4, {40, 20},false},
            {olc::Key::R, "R", 5, {60, 60},true},
            {olc::Key::K5, "5", 6, {80, 20},false},
            {olc::Key::T, "T", 7, {100, 60},true},
            {olc::Key::S, "S", 8, {140, 60},true},
            {olc::Key::X, "X", 9, {160, 20},false},
            {olc::Key::D, "D",10, {180, 60},true},
            {olc::Key::C, "C",11, {200, 20},false},
            {olc::Key::F, "F",12, {220, 60},true},
            {olc::Key::V, "V",13, {240, 20},false},
            {olc::Key::G, "G",14, {260, 60},true},
            {olc::Key::Y, "Y",15, {20, 140},true},
            {olc::Key::K7, "7",16, {40, 100},false},
            {olc::Key::U, "U",17, {60, 140},true},
            {olc::Key::K8, "8",18, {80, 100},false},
            {olc::Key::I, "I",19, {100, 140},true},
            {olc::Key::H, "H",20, {140, 140},true},
            {olc::Key::N, "N",21, {160, 100},false},
            {olc::Key::J, "J",22, {180, 140},true},
            {olc::Key::M, "M",23, {200, 100},false},
            {olc::Key::K, "K",24, {220, 140},true},
            {olc::Key::COMMA, ",",25,{240, 100},false},
            {olc::Key::L, "L",26, {260, 140},true},
			{olc::Key::O, "O",27, {20, 180},true},
			{olc::Key::P, "P",28, {60, 180},true},
			{olc::Key::K9, "9",29, {100, 180},true},
			{olc::Key::K0, "0",30, {140, 180},true},
			{olc::Key::Q, "Q",31, {180, 180},true},
			{olc::Key::W, "w",32, {220, 180},true},
			{olc::Key::K1, "1",33, {260, 180},true},
			{olc::Key::K2, "2",34, {300, 180},true}
        };
		
		return true;
	}
	
	bool playSong = false;
	float songTime = 0;
	int songIndex=0;
	int bpm = 400;
	
	int playedNotes[20];
	float playedNotesTimer[20];
	int noteIndex=0;
	
	//not in use?
	int loopcounter=0;
	float timer =0;
	//
	bool OnUserUpdate(float fElapsedTime) override
	{
		timer = std::max(0.0f,timer-fElapsedTime);
		
		// Custom wave drawing area (100x100, shifted 20px down and left from top-right)
		int waveBoxX = ScreenWidth() - 120;  // Shifted 20px left from right edge
		int waveBoxY = 20;                   // Shifted 20px down from top
		int waveBoxSize = 100;
		
		// Handle mouse input for custom wave drawing
		int mouseX = GetMouseX();
		int mouseY = GetMouseY();
		
		// Check if mouse is in the wave drawing box
		if (mouseX >= waveBoxX && mouseX < waveBoxX + waveBoxSize &&
			mouseY >= waveBoxY && mouseY < waveBoxY + waveBoxSize) {
			
			if (GetMouse(0).bPressed) {
				isDrawingWave = true;
				lastDrawX = -1;
			}
			
			if (GetMouse(0).bHeld && isDrawingWave) {
				int relX = mouseX - waveBoxX;
				int relY = mouseY - waveBoxY;
				
				// Convert Y position to value between -0.5 and 0.5
				float yValue = 0.5f - (relY / float(waveBoxSize));
				yValue = std::max(-0.5f, std::min(0.5f, yValue));
				
				// If we have a last position, interpolate between points for smooth drawing
				if (lastDrawX != -1 && lastDrawX != relX) {
					int startX = std::min(lastDrawX, relX);
					int endX = std::max(lastDrawX, relX);
					float lastY = customWavePoints[lastDrawX];
					
					for (int x = startX; x <= endX; x++) {
						if (x >= 0 && x < 100) {
							float t = (x - lastDrawX) / float(relX - lastDrawX);
							customWavePoints[x] = lastY + t * (yValue - lastY);
						}
					}
				} else {
					// Just set the current point
					if (relX >= 0 && relX < 100) {
						customWavePoints[relX] = yValue;
					}
				}
				
				lastDrawX = relX;
			}
		}
		
		if (GetMouse(0).bReleased) {
			isDrawingWave = false;
			lastDrawX = -1;
		}
		
		// Handle F2 key - use custom drawn wave
		if (GetKey(olc::Key::F2).bPressed) {
			// Create a new waveform based on the drawn shape
			for (int waveIndex = 0; waveIndex < counter; waveIndex++) {
				float freq = 110.0 * pow(2.0, waveIndex / 12.0)/2.0; // shiould update so that changing the 12 only has to be done in one place Note the one in onusercreate
				int samplesPerWave = 44100 / freq;
				
				for (size_t i = 0; i < 44100; i++) {
					double dt = 1.0 / 44100.0;
					double t = double(i) * dt;
					
					// Get position within the current wavelength
					float wavePosition = fmod(i, samplesPerWave) / samplesPerWave;
					int customIndex = int(wavePosition * 100);
					customIndex = std::min(99, customIndex);
					
					// Get the custom wave value
					float y = customWavePoints[customIndex];
					
					// Apply envelope
					y *= exp(-6 * t);
					y *= std::min(1.0, exp(-80 * (t - 0.97)));
					y *= std::min(1.0, exp(300 * (t - 0.005)));
					
					wavelist[waveIndex].file.data()[i] = y;
				}
			}
		}
		
		// Handle F3 key - reset custom wave to current reset type
		if (GetKey(olc::Key::F3).bPressed) {
			for (int i = 0; i < 100; i++) {
				float t = i / 99.0f; // Normalized position 0 to 1
				
				switch (resetWaveType) {
					case 0: // None (zero)
						customWavePoints[i] = 0.0f;
						break;
						
					case 1: // Sine wave
						customWavePoints[i] = 0.5f * sin(2.0f * 3.14159f * t);
						break;
						
					case 2: // Triangle wave
						if (t < 0.25f) {
							customWavePoints[i] = 2.0f * t;
						} else if (t < 0.75f) {
							customWavePoints[i] = 0.5f - 2.0f * (t - 0.25f);
						} else {
							customWavePoints[i] = -0.5f + 2.0f * (t - 0.75f);
						}
						break;
						
					case 3: // Square wave
						customWavePoints[i] = (t < 0.5f) ? 0.4f : -0.4f;
						break;
				}
			}
		}
		
		// Handle F4 key - cycle through reset wave types
		if (GetKey(olc::Key::F4).bPressed) {
			resetWaveType = (resetWaveType + 1) % 4;
		}
			
		//std::cout << fElapsedTime << "\n";
		
		if (GetKey(olc::Key::F1).bPressed) {
			playSong=true;
		
		}
		
		
		if (GetKey(olc::Key::F5).bPressed) {
			playSong=true;
		
		}
		
		for (int i =0; i <noteIndex; i++){
				
			playedNotesTimer[i] += fElapsedTime;
			
			if (playedNotesTimer[i] >0.5){
				
				
				//remove it
				for (int j =i; j <noteIndex-1; j++){
				playedNotes[j] =  playedNotesTimer[j+1];
				playedNotesTimer[j] =  playedNotesTimer[j+1];
				
				}
				playedNotes[noteIndex-1] =  0;
				playedNotesTimer[noteIndex-1] =0;
				
				i--;
				noteIndex--;
			}
				
		}
		
		
		if(playSong){
			songTime+= fElapsedTime;
			
			for (int i = songIndex; i<  Song.size(); i++){
				if(Song[i].start*(60.0/bpm)<=songTime){
					engine.PlayWaveform(&wavelist[Song[i].waveformIndex]);
					songIndex++;
					playedNotes[noteIndex] = Song[i].waveformIndex;
					playedNotesTimer[noteIndex] =fElapsedTime;
					noteIndex++;
					
				}
				else{
					break;
				}
			}
			
			if(songIndex>= Song.size()){
				playSong = false;
				songTime=0;
				songIndex=0;
				
			}
			
		}
		
		

		
		Clear(olc::BLACK);
		
		// Draw the custom wave drawing box
    // Draw background
    FillRect(waveBoxX, waveBoxY, waveBoxSize, waveBoxSize, olc::VERY_DARK_GREY);
    
    // Draw border
    DrawRect(waveBoxX, waveBoxY, waveBoxSize, waveBoxSize, olc::WHITE);
    
    // Draw center line (y = 0)
    int centerY = waveBoxY + waveBoxSize / 2;
    DrawLine(waveBoxX, centerY, waveBoxX + waveBoxSize, centerY, olc::DARK_GREY);
    
    // Draw max and min lines
    DrawLine(waveBoxX, waveBoxY, waveBoxX + waveBoxSize, waveBoxY, olc::DARK_RED);
    DrawLine(waveBoxX, waveBoxY + waveBoxSize - 1, waveBoxX + waveBoxSize, 
             waveBoxY + waveBoxSize - 1, olc::DARK_RED);
    
    // Draw the custom wave
    for (int x = 1; x < 100; x++) {
        int y1 = waveBoxY + waveBoxSize / 2 - customWavePoints[x - 1] * waveBoxSize;
        int y2 = waveBoxY + waveBoxSize / 2 - customWavePoints[x] * waveBoxSize;
        
        y1 = std::max(waveBoxY, std::min(waveBoxY + waveBoxSize - 1, y1));
        y2 = std::max(waveBoxY, std::min(waveBoxY + waveBoxSize - 1, y2));
        
        DrawLine(waveBoxX + x - 1, y1, waveBoxX + x, y2, olc::CYAN);
    }
    
    // Draw labels
    DrawString(waveBoxX, waveBoxY - 10, "Custom Wave", olc::WHITE, 1);
    DrawString(waveBoxX - 35, waveBoxY + 5, "+0.5", olc::WHITE, 1);
    DrawString(waveBoxX - 35, centerY - 3, " 0.0", olc::WHITE, 1);
    DrawString(waveBoxX - 35, waveBoxY + waveBoxSize - 7, "-0.5", olc::WHITE, 1);
    
    // Draw instructions
    DrawString(waveBoxX, waveBoxY + waveBoxSize + 5, "F2: Apply", olc::YELLOW, 1);
    
    // Display current reset type
    std::string resetTypeStr;
    switch (resetWaveType) {
        case 0: resetTypeStr = "None"; break;
        case 1: resetTypeStr = "Sine"; break;
        case 2: resetTypeStr = "Tri"; break;
        case 3: resetTypeStr = "Square"; break;
    }
    DrawString(waveBoxX, waveBoxY + waveBoxSize + 15, "F3: Reset (" + resetTypeStr + ")", olc::YELLOW, 1);
    DrawString(waveBoxX, waveBoxY + waveBoxSize + 25, "F4: Change Type", olc::YELLOW, 1);
    
		
		
		float yCoords[640];
		int PressCounter =0;
		
		for (int xCoord =0; xCoord < 640; xCoord++){
			yCoords[xCoord] =0;
		}
		
        for (const auto& btn : keyButtons) {
            bool pressed = GetKey(btn.key).bHeld;
			
			for (int i =0; i <noteIndex; i++){
				
				if(playedNotes[i] == btn.waveformIndex){
					pressed = true;
				}
				
			}
			
            olc::Pixel color = pressed ? olc::GREEN : olc::WHITE;
            olc::vi2d size = {30, 30};

            // Draw box
            FillRect(btn.position, size, color);

            // Draw border
            DrawRect(btn.position, size, olc::DARK_GREY);

            // Draw label centered
            DrawString(btn.position + olc::vi2d(10, 10), btn.label, olc::BLACK);

            // Optional: Play waveform on press (like your original logic)
            if (GetKey(btn.key).bPressed) {
                engine.PlayWaveform(&wavelist[btn.waveformIndex]);  
            }
			
			

        }
		for (const auto& btn : keyButtons) {
		bool pressed = GetKey(btn.key).bHeld;
		
			for (int i =0; i <noteIndex; i++){
				
				if(playedNotes[i] == btn.waveformIndex){
					pressed = true;
				}
				
			}
		
		//draw waveform 
			if(pressed){
				PressCounter++;
				
				for (int xCoord =0; xCoord < 640; xCoord++){
					
					float yFloatChord = std::min(std::max(-1.0f,wavelist[btn.waveformIndex].file.data()[2*xCoord*(4410/640)]),1.0f);
					//std::cout<< std::to_string(wavelist[btn.waveformIndex].file.data()[xCoord]);
					int yCoord = 300 + yFloatChord * 100;
					//DrawLine(xCoord,yCoord,xCoord-1,lastPoint);
					
					yCoords[xCoord]+= yFloatChord;
					
				} 
			}
		}
		
		
		DrawLine(0,350,640,350,olc::RED);
		DrawLine(0,300,640,300,olc::GREEN);
		DrawLine(0,250,640,250,olc::RED);
		
		int lastPoint =0;
		for (int xCoord =0; xCoord < 640; xCoord++){
			int yCoord = 300 + yCoords[xCoord]/PressCounter * -100;
			DrawLine(xCoord,yCoord,xCoord-1,lastPoint);
			lastPoint = yCoord;
		}
		
		
		/* if ((loopcounter==0 &&GetKey(olc::Key::K3).bPressed) || (loopcounter !=0 && timer <= 0)){  //DO NOT USE K3 AS BUTTON BUT KEEP AROUND IN CASE 
			
			
			engine.PlayWaveform(&wavelist[loopcounter]);
			loopcounter++;
			timer=0.5;
			if(loopcounter == counter){
				loopcounter=0;
			}
		} */
		
		
		
		
		
		
		DrawString({ 4,  4 }, "[1] Play 44100Hz x 8-bit Custom Sample");		
		return true;
	}
};



int main()
{
	Example004 demo;
	if (demo.Construct(640, 480, 2, 2))
		demo.Start();
	return 0;
}