class MowerCommand {
  public:
    MowerCommand();
    MowerCommand(int newMode);

    int getNewMode();
    char* toJson();
    void fromJson(char * jsonInput);
    bool isValid;
  private:
    int newMode;    
};
