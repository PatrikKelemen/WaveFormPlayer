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
	olc::sound::Wave wavelist[50];
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
	
public:
	bool OnUserCreate() override
	{
		engine.InitialiseAudio(44100, 1);

		custom_wave = olc::sound::Wave(1, sizeof(uint8_t), 44100, 44100);
		
		
		
		
		for (float freq = 220.0;freq <=1760.0;freq*=1.05946309463){
			
			wavelist[counter]= olc::sound::Wave(1, sizeof(uint8_t), 44100, 44100);
			for (size_t i = 0; i < 44100; i++){
				double dt = 1.0 / 44100.0;
				double t = double(i) * dt;
				
				float y = float(0.3 * sin(0.5 * freq * 3.14159 * t) * exp(-0.01 * freq *t));
				
				y += float(0.2 *sin(1 * freq * 3.14159 * t) * exp(-0.01 * freq*t) );
				y += float(0.1 *sin(0.05 * freq * 3.14159 * t) * exp(-0.01 * freq*t) );
				//y += float(0.02 *sin(0.5 * freq * 3.14159 * t) * exp(-0.008 * freq*t) );
				y = y*y*y;
				y += float(0.3 * sin(0.5 * freq * 3.14159 * t) * exp(-6 *t));
				y += float(0.2 *sin(1 * freq * 3.14159 * t) * exp(-6*t) );
				y *=  1+16*t*exp(-12*t);
				//y *=  1+16*t*exp(-12*t);
				//y *= 1- ((double) rand() / (RAND_MAX))/(4*exp(-3*(t-1))) ;
				
				
				//square wave test 
				if(sin(1 * freq * 3.14159 * t) >.9){
					y=0.5* exp(-6 *t);
				}
				else {
					y=-0.5* exp(-6 *t);
				}
				
				
				y *= std::min(1.0,exp(-80*(t-0.97))); //reset end position to zero
				y *= std::min(1.0,exp(300*(t-0.01))); //force start position to come in slower 
				
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
            {olc::Key::L, "L",26, {260, 140},true}
        };
		
		return true;
	}
	
	
	int loopcounter=0;
	float timer =0;
	
	bool OnUserUpdate(float fElapsedTime) override
	{
		timer = std::max(0.0f,timer-fElapsedTime);
		
		if (GetKey(olc::Key::K1).bPressed)
			engine.PlayWaveform(&custom_wave);
		if (GetKey(olc::Key::K2).bPressed)
			engine.PlayWaveform(&wavelist[7]);
		
		
		
		
		
		

		
		Clear(olc::BLACK);

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
		
		/* if ((loopcounter==0 &&GetKey(olc::Key::K3).bPressed) || (loopcounter !=0 && timer <= 0)){
			
			
			engine.PlayWaveform(&wavelist[loopcounter]);
			loopcounter++;
			timer=0.5;
			if(loopcounter == counter){
				loopcounter=0;
			}
		} */
		
		
		//draw waveform 
		
		
		
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