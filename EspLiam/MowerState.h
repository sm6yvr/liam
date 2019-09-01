
class MowerState {
  public:
    MowerState();
    MowerState(int mode, int batteryVoltage, bool isLeftOutside);

    int getMode();
    char* toJson();
    void fromJson(char * jsonInput);
    bool isValid;
  private:
    int mode;
    int batteryVoltage;
    bool isLeftOutside;
    
};
