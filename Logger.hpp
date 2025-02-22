#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <ostream>
#include <sstream>
#include <string>
#include <thread>
#include <chrono>
#include <sys/ioctl.h> 
#include <unistd.h> 
#include <iostream>

//Regular text
#define BLK "\e[0;30m"
#define RED "\e[0;31m"
#define GRN "\e[0;32m"
#define YEL "\e[0;33m"
#define BLU "\e[0;34m"
#define MAG "\e[0;35m"
#define CYN "\e[0;36m"
#define WHT "\e[0;37m"

//Regular bold text
#define BBLK "\e[1;30m"
#define BRED "\e[1;31m"
#define BGRN "\e[1;32m"
#define BYEL "\e[1;33m"
#define BBLU "\e[1;34m"
#define BMAG "\e[1;35m"
#define BCYN "\e[1;36m"
#define BWHT "\e[1;37m"

//Regular underline text
#define UBLK "\e[4;30m"
#define URED "\e[4;31m"
#define UGRN "\e[4;32m"
#define UYEL "\e[4;33m"
#define UBLU "\e[4;34m"
#define UMAG "\e[4;35m"
#define UCYN "\e[4;36m"
#define UWHT "\e[4;37m"

//Regular background
#define BLKB "\e[40m"
#define REDB "\e[41m"
#define GRNB "\e[42m"
#define YELB "\e[43m"
#define BLUB "\e[44m"
#define MAGB "\e[45m"
#define CYNB "\e[46m"
#define WHTB "\e[47m"

//High intensty background 
#define BLKHB "\e[0;100m"
#define REDHB "\e[0;101m"
#define GRNHB "\e[0;102m"
#define YELHB "\e[0;103m"
#define BLUHB "\e[0;104m"
#define MAGHB "\e[0;105m"
#define CYNHB "\e[0;106m"
#define WHTHB "\e[0;107m"

//High intensty text
#define HBLK "\e[0;90m"
#define HRED "\e[0;91m"
#define HGRN "\e[0;92m"
#define HYEL "\e[0;93m"
#define HBLU "\e[0;94m"
#define HMAG "\e[0;95m"
#define HCYN "\e[0;96m"
#define HWHT "\e[0;97m"

//Bold high intensity text
#define BHBLK "\e[1;90m"
#define BHRED "\e[1;91m"
#define BHGRN "\e[1;92m"
#define BHYEL "\e[1;93m"
#define BHBLU "\e[1;94m"
#define BHMAG "\e[1;95m"
#define BHCYN "\e[1;96m"
#define BHWHT "\e[1;97m"

//Reset
#define RST "\e[0m"

namespace Log {
    #define __FILENAME__ (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)


    class Progressbar {
        #define PERCENTAGE_MESSAGE_MAX_SIZE 6
        #define TIME_MESSAGE_DEFAULT_SIZE 6
        private:
            constexpr static int calculatePercentage(const double& amount, const double& total) noexcept {
                return (amount/total) * 100;
            }

            constexpr static int min(const int& a, const int& b) noexcept {
                return a < b ? a : b;
            }

            static unsigned int getProgressbarMaxWidth(const unsigned int& messageSize, const unsigned int& timeMessageSize) noexcept {
                #if defined _WIN32
                    #include <windows.h>
                    CONSOLE_SCREEN_BUFFER_INFO csbi;
                    int columns;
                    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
                    columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
                    return columns - messageSize - (timeMessageSize + TIME_MESSAGE_DEFAULT_SIZE) - PERCENTAGE_MESSAGE_MAX_SIZE;
                #else 
                    struct winsize size;
                    ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
                    return size.ws_col - messageSize - (timeMessageSize + TIME_MESSAGE_DEFAULT_SIZE) - PERCENTAGE_MESSAGE_MAX_SIZE;
                #endif
            }

            static void clearInformationFromStream(std::ostream& os, const int& num) noexcept {
                for(int i = 0; i < num; i++){
                    os << "\b";
                }
            }

            static std::string getTimeStringFromSeconds(const int &number) noexcept {
              std::stringstream parser;
              if ((number / 100) + (number % 100) / 60 < 10) {
                  parser << "0"; // trailing 0 if minutes < 10
              }
              parser << (number / 100) + (number % 100) / 60 << ':';                                 
              if ((number % 100) % 60 < 10) {
                  parser << "0"; // trailing 0 if seconds < 10
              }
              parser << (number % 100) % 60;
              return parser.str();
            }

            static std::string constructPercentageIndicator(const int& percentage) noexcept {
                return "{" + std::to_string(percentage) + "%}";
            }
            static std::string constructTimeRemaining(const int& timeRemaining) noexcept {
                return "[" + getTimeStringFromSeconds(timeRemaining) + "]"; 
            }

            static void displayProgressBar(std::ostream &os, unsigned int width,
                                          const std::string &msg = "Loading",
                                          const unsigned int &waitAmount = 100,
                                          const std::string &loadingSymbol = "━") {
             unsigned short iPercentage = 0;

              // Keep track of time remaining
              unsigned int iTimeRemaining = width;

              // String containers for keeping track of the size of the string
              std::string sPercentage = "";
              std::string sTimeRemaining = constructTimeRemaining(iTimeRemaining);

              // Get the possible max width of the progress bar using current
              // terminal width
              unsigned int maxWidth =
                  getProgressbarMaxWidth(msg.size(), sTimeRemaining.size());

              // Determine if width given is greater than current terminal width
              iTimeRemaining = iTimeRemaining > maxWidth ? maxWidth : width;
              width = iTimeRemaining;
              
              // Set initial stream color to red
              os << msg << " ";

              for (int count = 0; count < width; ++count) {
                iPercentage = calculatePercentage(count, width);

                sPercentage = constructPercentageIndicator(iPercentage);
                sTimeRemaining = constructTimeRemaining(iTimeRemaining--);

                // Display percentage as blue
                os << loadingSymbol << " " << sPercentage;

                // Display time remaining as magenta
                os << " " << sTimeRemaining;

                // Flush stream
                os.flush();

                // Remove information data to prepare for next iteration
                clearInformationFromStream(os, sPercentage.size() +
                                                   sTimeRemaining.size() + 2);

                // Sleep the thread to simulate loading
                std::this_thread::sleep_for(std::chrono::milliseconds(waitAmount));
              }

              // Reset cursor to the start of stream
              os << "\r";

              // Reset stream color and set to Green
              os << msg << " ";

              // Overwrite the current stream to display full progress bar
              for (int count = 0; count < width + 3; ++count) {
                os << loadingSymbol;
              }

              // Display information for a completed progress bar
              os << " " << constructPercentageIndicator(100) << " "
                 << constructTimeRemaining(0) << "\n";

            }

       public:
            static void createProgressBar(std::ostream& os, 
                    const std::string& msg = "Loading", 
                    const bool& useColor = true,
                    const unsigned int& waitAmount=100, 
                    const std::string& loadingSymbol = "━") {
                createProgressBar(os,50,msg,useColor,waitAmount,loadingSymbol);
            } 

            // Creates and displays a terminal progress bar
            static void createProgressBar(std::ostream &os, unsigned int width,
                                         const std::string &msg = "Loading",
                                          const bool& useColor = true,
                                          const unsigned int &waitAmount = 100,
                                          const std::string &loadingSymbol = "━") {

              if(!useColor) {
                  displayProgressBar(os, width,msg,waitAmount,loadingSymbol);
                  return;
              }

              // Keep track of percentage
              unsigned short iPercentage = 0;

              // Keep track of time remaining
              unsigned int iTimeRemaining = width;

              // String containers for keeping track of the size of the string
              std::string sPercentage = "";
              std::string sTimeRemaining = constructTimeRemaining(iTimeRemaining);

              // Get the possible max width of the progress bar using current
              // terminal width
              unsigned int maxWidth =
                  getProgressbarMaxWidth(msg.size(), sTimeRemaining.size());

              // Determine if width given is greater than current terminal width
              iTimeRemaining = iTimeRemaining > maxWidth ? maxWidth : width;
              width = iTimeRemaining;

              // Set initial stream color to red
              os << msg << " " << BRED;

              for (int count = 0; count < width; ++count) {
                iPercentage = calculatePercentage(count, width);

                sPercentage = constructPercentageIndicator(iPercentage);
                sTimeRemaining = constructTimeRemaining(iTimeRemaining--);

                // Display percentage as blue
                os << loadingSymbol << " " << BLU << sPercentage << RST;

                // Display time remaining as magenta
                os << " " << MAG << sTimeRemaining << BRED;

                // Flush stream
                os.flush();

                // Remove information data to prepare for next iteration
                clearInformationFromStream(os, sPercentage.size() +
                                                   sTimeRemaining.size() + 2);

                // Sleep the thread to simulate loading
                std::this_thread::sleep_for(std::chrono::milliseconds(waitAmount));
              }

              // Reset cursor to the start of stream
              os << "\r";

              // Reset stream color and set to Green
              os << RST << msg << " " << GRN;

              // Overwrite the current stream to display full progress bar
              for (int count = 0; count < width + 3; ++count) {
                os << loadingSymbol;
              }

              // Display information for a completed progress bar
              os << " " << constructPercentageIndicator(100) << " "
                 << constructTimeRemaining(0) << RST << "\n";
            }
    };

    class Clock {
        public:
            friend std::ostream& operator<<(std::ostream& output, const Log::Clock& l) {

                // Get current system time
                std::time_t t = std::time(0);
                std::tm *ct = std::localtime(&t);

                // Extract seconds, minutes and hours from struct
                int seconds = ct->tm_sec;
                int minutes = ct->tm_min;
                int hours = ct->tm_hour;

                // Format time string
                if(output.rdbuf() == std::cout.rdbuf()) {
                    output << MAG << "{" << (hours<10?"0":"") << hours <<":" << (minutes<10?"0":"") << minutes << ":" << (seconds<10?"0":"") << seconds << "} " << RST;
                } else {
                    output << "{" << (hours<10?"0":"") << hours <<":" << (minutes<10?"0":"") << minutes << ":" << (seconds<10?"0":"") << seconds << "} ";
                }

                return output;
            }
    };

    class Debug {
        #define DEBUG Log::Clock() << GRN << "[DEBUG]    {" << __FILENAME__ << "}" << RST << "   "
        #define DEBUG_NO_COLOR Log::Clock() << "[DEBUG]    {" << __FILENAME__ << "}" << "   "
        public:
            friend std::ostream &operator<<(std::ostream &output, const Log::Debug& l) { 
                if(output.rdbuf() == std::cout.rdbuf()) {
                    output << DEBUG;
                } else {
                    output << DEBUG_NO_COLOR;
                }
                return output;            
            }
    };

    class Info {
        #define INFO Log::Clock() << BLU << "[INFO]     {" << __FILENAME__ << "}" << RST << "   "
        #define INFO_NO_COLOR Log::Clock() << "[INFO]     {" << __FILENAME__ << "}" << "   "
        public:
            friend std::ostream &operator<<(std::ostream &output, const Log::Info& l) { 
                if(output.rdbuf() == std::cout.rdbuf()) {
                    output << INFO;
                } else {
                    output << INFO_NO_COLOR;
                }
                return output;            
            }
    };

    class Warning {
        #define WARNING Log::Clock() << YEL << "[WARNING]  {" << __FILENAME__ << "}" << RST << "   "
        #define WARNING_NO_COLOR Log::Clock() << "[WARNING]  {" << __FILENAME__ << "}" << "   "
        public:
            friend std::ostream &operator<<(std::ostream &output, const Log::Warning& l) { 
                if(output.rdbuf() == std::cout.rdbuf()) {
                    output << WARNING;
                } else {
                    output << WARNING_NO_COLOR;
                }
                return output;            
            }
    };

    class Error {
        #define ERROR Log::Clock() << HRED << "[ERROR]    {" << __FILENAME__ << "}" << RST << "   "
        #define ERROR_NO_COLOR Log::Clock() << "[ERROR]    {" << __FILENAME__ << "}" << "   "
        public:
            friend std::ostream &operator<<(std::ostream &output, const Log::Error& l) { 
                if(output.rdbuf() == std::cout.rdbuf()) {
                    output << ERROR;
                } else {
                    output << ERROR_NO_COLOR;
                }
                return output;            
            }
    };

    class Critical {
        #define CRITICAL Log::Clock() << REDHB << "[CRITICAL] {" << __FILENAME__ << "}" << RST << "   "
        #define CRITICAL_NO_COLOR Log::Clock() << "[CRITICAL] {" << __FILENAME__ << "}" << "   "
        public:
            friend std::ostream &operator<<(std::ostream &output, const Log::Critical& l) { 
                if(output.rdbuf() == std::cout.rdbuf()) {
                    output << CRITICAL;
                } else {
                    output << CRITICAL_NO_COLOR;
                }
                return output;            
            }
    };

};

#endif
