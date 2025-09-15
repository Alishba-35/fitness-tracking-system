#include <iostream>
#include <fstream>
#include <string>
#include <limits>
#include <windows.h>
#include <stdexcept>

using namespace std;

const int WIDTH = 80;

// ------ Utility Functions ------
void centerText(const string& text) {
    int pad = max(0, (WIDTH - (int)text.length()) / 4);
    cout << string(pad, ' ') << text << "\n";
}

void printDivider() {
    cout << string(WIDTH, '-') << "\n";
}

void loadingAnimation(const string& msg = "Loading") {
    cout << msg;
    for (int i = 0; i < 3; ++i) {
        Sleep(300);
        cout << ".";
    }
    cout << "\n";
}

template<typename T>
T getValidatedInput(const string& prompt) {
    T val;
    while (true) {
        cout << prompt;
        cin >> val;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            centerText("Invalid input, try again.");
        } else {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return val;
        }
    }
}

// ------ Template Logger Class ------
template<typename T>
class Logger {
    string filename;

public:
    Logger(const string& file) : filename(file) {}

    void log(const T& data) const {
        ofstream fout(filename, ios::app);
        if (!fout) throw runtime_error("Cannot open file to log data.");
        fout << data << "\n";
    }

    void display() const {
        ifstream fin(filename);
        if (!fin) {
            centerText("No records found.");
            return;
        }
        T value;
        int count = 1;
        while (fin >> value) {
            cout << "Entry " << count++ << ": " << value << "\n";
        }
    }
};

// ------ Classes ------
class User {
protected:
    string username, password, routine;
    float weight;
    int hFeet, hInches, age;

public:
    User() : weight(0), hFeet(0), hInches(0), age(0) {}

    void inputData() {
        printDivider();
        centerText("Register New User");
        printDivider();
        cout << "Username: "; cin >> username;
        cout << "Password: "; cin >> password;
        weight = getValidatedInput<float>("Weight (kg): ");
        hFeet = getValidatedInput<int>("Height - Feet: ");
        hInches = getValidatedInput<int>("Height - Inches: ");
        age = getValidatedInput<int>("Age (years): ");
        cout << "Routine (sedentary/moderate/active): "; cin >> routine;
        centerText("Registration completed!");
    }

    bool login() {
        printDivider();
        centerText("User Login");
        printDivider();
        cout << "Username: "; cin >> username;
        cout << "Password: "; cin >> password;

        ifstream fin(username + "_profile.txt");
        if (!fin) {
            centerText("No such user found.");
            return false;
        }
        getline(fin, username);
        getline(fin, password);
        fin >> weight >> hFeet >> hInches >> age;
        fin.ignore();
        getline(fin, routine);
        fin.close();

        centerText("Login successful!");
        return true;
    }

    void showProfile() const {
        printDivider();
        centerText("User Profile");
        printDivider();
        cout << "Username: " << username << "\n"
             << "Weight: " << weight << " kg\n"
             << "Height: " << hFeet << " ft " << hInches << " in\n"
             << "Age: " << age << "\n"
             << "Routine: " << routine << "\n";
    }

    void saveProfile() const {
        ofstream fout(username + "_profile.txt");
        if (!fout) throw runtime_error("Can't save profile.");
        fout << username << "\n"
             << password << "\n"
             << weight << "\n"
             << hFeet << "\n"
             << hInches << "\n"
             << age << "\n"
             << routine << "\n";
    }

    void loadProfile() {
        ifstream fin(username + "_profile.txt");
        if (!fin) throw runtime_error("Can't load profile.");
        getline(fin, username);
        getline(fin, password);
        fin >> weight >> hFeet >> hInches >> age;
        fin.ignore();
        getline(fin, routine);
    }

    float calculateBMI() const {
        float meters = ((hFeet * 12) + hInches) * 0.0254f;
        return weight / (meters * meters);
    }

    string getRoutine() const { return routine; }
    string getUsername() const { return username; }
    float getWeight() const { return weight; }
};

// Goal Class
class Goal {
    string desc;
    float targetW;

public:
    void inputGoal() {
        printDivider();
        centerText("Set Fitness Goal");
        printDivider();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Goal Description: ";
        getline(cin, desc);
        targetW = getValidatedInput<float>("Target Weight (kg): ");
    }

    void showGoal() const {
        printDivider();
        centerText("Fitness Goal");
        printDivider();
        cout << "Goal: " << desc << "\n"
             << "Target Weight: " << targetW << " kg\n";
    }

    void saveGoal(const string& user) const {
        ofstream fout(user + "_goal.txt");
        if (!fout) throw runtime_error("Can't save goal.");
        fout << desc << "\n" << targetW << "\n";
    }

    void loadGoal(const string& user) {
        ifstream fin(user + "_goal.txt");
        if (!fin) throw runtime_error("Can't load goal.");
        getline(fin, desc);
        fin >> targetW;
    }

    float getTargetW() const { return targetW; }
};

// Tracker Class
class Tracker : public User {
    Goal goal;

public:
    void inputAll() {
        inputData();
        goal.inputGoal();
    }

    void loadAll() {
        loadProfile();
        goal.loadGoal(getUsername());
    }

    void saveAll() const {
        saveProfile();
        goal.saveGoal(getUsername());
    }

    void checkWeight() const {
        printDivider();
        centerText("BMI Result");
        printDivider();
        float bmi = calculateBMI();
        cout << "Your BMI: " << bmi << "\n";

        if (bmi < 18.5) centerText("Underweight. Consider gaining weight.");
        else if (bmi < 25) centerText("Normal weight. Great job!");
        else if (bmi < 30) centerText("Overweight. Consider losing weight.");
        else centerText("Obesity alert. Consult doctor.");

        logBMIHistory();
    }

    void logBMIHistory() const {
        Logger<float> bmiLogger(getUsername() + "_bmi.txt");
        bmiLogger.log(calculateBMI());
    }

    void viewBMIHistory() const {
        printDivider();
        centerText("BMI History");
        printDivider();
        Logger<float> bmiLogger(getUsername() + "_bmi.txt");
        bmiLogger.display();
    }

    void suggestRestDays() const {
        printDivider();
        centerText("Rest Day Suggestion");
        printDivider();
        string r = getRoutine();
        for (auto& c : r) c = tolower(c);
        if (r == "active")
            centerText("Rest 1 day per week. Example: Sunday");
        else if (r == "moderate")
            centerText("Rest 2 days per week. Example: Wednesday & Sunday");
        else
            centerText("Rest 3 days per week. Example: Monday, Wednesday & Friday");
    }

    void logNutrition() const {
        printDivider();
        centerText("Nutrition Logging");
        printDivider();
        ofstream fout(getUsername() + "_nutrition.txt", ios::app);
        if (!fout) throw runtime_error("Can't open nutrition file.");
        while (true) {
            string food;
            cout << "Food (egg/banana/milk or 'done'): ";
            cin >> food;
            if (food == "done") break;
            int cal = getValidatedInput<int>("Calories: ");
            fout << food << " - " << cal << " cal\n";
        }
    }

    void viewNutritionLog() const {
        printDivider();
        centerText("Nutrition Log");
        printDivider();
        ifstream fin(getUsername() + "_nutrition.txt");
        if (!fin) { centerText("No entries."); return; }
        string line;
        while (getline(fin, line)) cout << line << "\n";
    }

   void recommendExercise() const {
    printDivider();
    centerText("Exercise Plan");
    printDivider();
    float w = getWeight(), target = goal.getTargetW();
    
    if (w < target) {
        centerText("Aim to GAIN: Include strength training 3-4 times a week, and consider protein-rich foods.");
        centerText("Exercises: Squats, Deadlifts, Bench Press, and Bodyweight exercises.");
    } else if (w > target) {
        centerText("Aim to LOSE: Focus on cardio 4-5 times a week, and incorporate HIIT 2 times a week.");
        centerText("Exercises: Running, Cycling, Swimming, and Circuit Training.");
    } else {
        centerText("Maintain: Balanced routine with strength training and cardio 3 times a week.");
        centerText("Exercises: Mix of weight lifting and aerobic activities.");
    }
}

void recommendDiet() const {
    printDivider();
    centerText("Diet Advice");
    printDivider();
    float w = getWeight(), target = goal.getTargetW();
    
    if (w < target) {
        centerText("To gain weight, include: Protein shakes, nuts, avocados, and whole grains.");
        centerText("Meals: 5-6 smaller meals throughout the day.");
    } else if (w > target) {
        centerText("To lose weight, focus on: Lean proteins, vegetables, and whole foods.");
        centerText("Meals: 3 balanced meals with healthy snacks in between.");
    } else {
        centerText("To maintain weight, include: A balanced diet with all food groups.");
        centerText("Meals: Regular meals with portion control.");
    }
}


    void recommendWaterIntake() const {
        printDivider();
        centerText("Water Intake");
        printDivider();
        float liters = getWeight() * 0.033f;
        cout << "Drink at least " << liters << " L/day\n";
    }

    void viewSummaryReport() const {
        printDivider();
        centerText("Summary Report");
        printDivider();
        showProfile();
        goal.showGoal();
        checkWeight();
        recommendWaterIntake();
    }
};

// ------ Main ------
int main() {
    Tracker user;
    int choice;

    try {
        loadingAnimation("Starting App");
        printDivider();
        centerText("Welcome to Fitness Tracker");
        printDivider();

        centerText("1. Login");
        centerText("2. Register");
        cout << "Choice: ";
        cin >> choice;

        if (choice == 1) {
            if (!user.login()) return 0;
            user.loadAll();
        } else if (choice == 2) {
            user.inputAll();
            user.saveAll();
        } else {
            centerText("Invalid choice. Exiting.");
            return 0;
        }

        while (true) {
            printDivider();
            centerText("Main Menu");
            printDivider();
            centerText("1. View Profile");
            centerText("2. View Goal");
            centerText("3. Check BMI");
            centerText("4.    Recommend Exercise");
            centerText("5. Recommend Diet");
            centerText("6. Log Nutrition");
            centerText("7.    View Nutrition Log");
            centerText("8. Water Intake");
            centerText("9. Summary Report");
            centerText("10. View BMI History");
            centerText("11. Suggest Rest Days");
            centerText("12. Save & Exit");
            cout << "Choice: ";
            cin >> choice;

            switch (choice) {
                case 1: user.showProfile(); break;
                case 2: user.loadAll(); user.showProfile(); break;
                case 3: user.checkWeight(); break;
                case 4: user.recommendExercise(); break;
                case 5: user.recommendDiet(); break;
                case 6: user.logNutrition(); break;
                case 7: user.viewNutritionLog(); break;
                case 8: user.recommendWaterIntake(); break;
                case 9: user.viewSummaryReport(); break;
                case 10: user.viewBMIHistory(); break;
                case 11: user.suggestRestDays(); break;
                case 12: user.saveAll(); centerText("Saved. Goodbye!"); exit(0);
                default: centerText("Invalid input. Try again."); break;
            }
        }
    } catch (const exception& ex) {
        printDivider();
        centerText("An error occurred:");
        centerText(ex.what());
        printDivider();
    }

    return 0;
}

