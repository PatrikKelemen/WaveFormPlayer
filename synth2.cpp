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
	
	
	struct KeyButton {
        olc::Key key;
        std::string label;
        int waveformIndex;
        olc::vi2d position;
		bool white;
    };

    std::vector<KeyButton> keyButtons;
	
	float inverseExponentialWave(int exponent, int modifier, int wavePos){
	
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
	
public:
	bool OnUserCreate() override
	{
		engine.InitialiseAudio(44100, 1);

		custom_wave = olc::sound::Wave(1, sizeof(uint8_t), 44100, 44100);
		
		
		
		
		for (float freq = 110.0;freq <=1760.0 * (0.5);freq*=exp(log(2.0)/12.0)){  //change 12 to 31 to get 31 notes per octave and vice versa 
			
			wavelist[counter]= olc::sound::Wave(1, sizeof(uint8_t), 44100, 44100);
			for (size_t i = 0; i < 44100; i++){
				double dt = 1.0 / 44100.0;
				double t = double(i) * dt;
				
				float y = float(0.3 * sin(2 * freq * 3.14159 * t) );
				
				  y += float(0.3 * sin(1.0226*2 * freq * 3.14159 * t) );
				 // y += float(0.2 * sin(1.005*2 * freq * 3.14159 * t) );
				 // y += float(0.2 * sin(1.01*2 * freq * 3.14159 * t) );
				 //y += float(0.2 * sin((1.0/1.025)*2 * freq * 3.14159 * t) );
				 // y += float(0.2 * sin((1.0/1.005)*2 * freq * 3.14159 * t) );
				 // y += float(0.2 * sin((1.0/1.01)*2 * freq * 3.14159 * t) );
				//float y = float(0.3 * sin(1.2*2 * freq * 3.14159 * t) );
				
				
				
				y*= 0.8*exp(-4*t);
				//y += float(0.2 *sin(1 * freq * 3.14159 * t) * exp(-0.01 * freq*t) );
				//y += float(0.1 *sin(0.05 * freq * 3.14159 * t) * exp(-0.01 * freq*t) );
				//y += float(0.02 *sin(0.5 * freq * 3.14159 * t) * exp(-0.008 * freq*t) );
				//y = y*y*y;
				//y += float(0.3 * sin(0.5 * freq * 3.14159 * t) * exp(-6 *t));
				//y += float(0.2 *sin(1 * freq * 3.14159 * t) * exp(-6*t) );
				//y *=  1+16*t*exp(-12*t);
				//y *=  1+16*t*exp(-12*t);
				//y *= 1- ((double) rand() / (RAND_MAX))/(4*exp(-3*(t-1))) ;
				
				/*
				//square wave test 
				if(sin(1 * freq * 3.14159 * t) >.9){
					y=0.5* exp(-6 *t);
				}
				else {
					y=-0.5* exp(-6 *t);
				}
				//*/
				int modifier = int(44100/freq);
				
				int wavePos = i%modifier;
				//std::cout << wavePos << "\n"; 
				/* //triangle wave
				if (wavePos<modifier/4 ){
					y= (float(wavePos * 4 )* 1.0/float (modifier) * 0.4)* exp(-6*t)  ; 
					//std::cout<< std::to_string(float(wavePos * 4 )* (1/modifier) * 0.5) << "\n";
					//std::cout << std::to_string(y) << " : " << std::to_string(freq) << "\n"; 
				}
				
				
				else if (wavePos<3*modifier/4 ){
				y= (0.4 - float((wavePos - (modifier/4.0)) * 2.0 )* 1.0/float (modifier)* 0.8)* exp(-6*t)  ;
				}
				else {
					y = (-0.4 + float((wavePos - (3*modifier/4.0)) * 4.0 )* 1.0/float (modifier)* 0.4 )* exp(-6*t) ;
					
				}//*/
				
				
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
				 int exponent = 4;
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
				//*	
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
				
				y= 0.35* inverseExponentialWave( exponent,  modifier, wavePos)*exp(-3*t);
				y+= 0.25 * inverseExponentialWave( exponent,  int(44100/(2*freq)), i%(int(44100/(2*freq)))) *exp(-4*t);
				y+= 0.2 * inverseExponentialWave( exponent,  int(44100/(3*freq)), i%(int(44100/(3*freq)))) *exp(-5*t);
				y+= 0.15 * inverseExponentialWave( exponent,  int(44100/(4*freq)), i%(int(44100/(4*freq)))) *exp(-6*t);
				y+= 0.1 * inverseExponentialWave( exponent,  int(44100/(5*freq)), i%(int(44100/(5*freq)))) *exp(-7*t) ;
				y+= 0.05 * inverseExponentialWave( exponent,  int(44100/(6*freq)), i%(int(44100/(6*freq)))) *exp(-8*t);
				//y= y* exp(-6*t);
				//*/
				//y*= 0.8*exp(-4*t);
				y *= (1 + 3 * t * exp(-12 * t)); // Attack envelope - gives it that "punch"
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
	
	
	int loopcounter=0;
	float timer =0;
	
	bool OnUserUpdate(float fElapsedTime) override
	{
		timer = std::max(0.0f,timer-fElapsedTime);
		
		
		
		
		
		
		
		

		
		Clear(olc::BLACK);
		
		float yCoords[640];
		int PressCounter =0;
		
		for (int xCoord =0; xCoord < 640; xCoord++){
			yCoords[xCoord] =0;
		}
		
        for (const auto& btn : keyButtons) {
            bool pressed = GetKey(btn.key).bHeld;
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
			int yCoord = 300 + yCoords[xCoord]/PressCounter * 100;
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