
class Emulator {
private:
	
public:

	ROM rom;
	ppuBus* ppuBus = ppuBus::getInstance();
	void start();

};