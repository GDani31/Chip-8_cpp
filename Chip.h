#include <vector>
#include <string>

class Chip
{
private:
    std::vector<unsigned short int> memory;
    std::vector<unsigned short int> V;
    std::vector<unsigned short int> stack;
    int SP;
    unsigned short int I;
    unsigned short int PC;
    int delay_timer;
    int sound_timer;
    std::vector<unsigned short int> keypad;
    std::vector<unsigned short int> display;
    bool drawFlag;

public:
    Chip();
    void run();
    std::vector<unsigned short int> getDisplay();
    bool needsRedraw();
    void removeDrawFlag();
    void setKeyBuffer(const std::vector<int>& keyBuffer);
    void loadFontset();
    void loadFile(const std::string& filepath);
};
