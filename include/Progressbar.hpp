#ifndef PROGRESS_HPP
#define PROGRESS_HPP

#include "./ANSIColorTable.h"

#include <ostream>
#include <sstream>
#include <string>
#include <thread>
#include <chrono>
#include <sys/ioctl.h> 
#include <unistd.h> 

#define PERCENTAGE_MESSAGE_MAX_SIZE 6
#define TIME_MESSAGE_DEFAULT_SIZE 6

class Progressbar {
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

        static void createProgressBarWithoutColour(std::ostream &os, unsigned int width,
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
              createProgressBarWithoutColour(os, width,msg,waitAmount,loadingSymbol);
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
          os << msg << " " << RED;

          for (int count = 0; count < width; ++count) {
            iPercentage = calculatePercentage(count, width);

            sPercentage = constructPercentageIndicator(iPercentage);
            sTimeRemaining = constructTimeRemaining(iTimeRemaining--);

            // Display percentage as blue
            os << loadingSymbol << " " << BLU << sPercentage << RST;

            // Display time remaining as magenta
            os << " " << MAG << sTimeRemaining << RED;

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
#endif
