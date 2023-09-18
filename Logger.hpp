

class Logger{
public:
    Logger();
    ~Logger() = default;
    void Log(ChessMove move);
    void PrintLog();
private:
    std::vector<ChessMove> log;
};