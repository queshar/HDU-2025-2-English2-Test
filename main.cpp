#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <limits>
#include <ctime>
#include <windows.h>
#include <numeric>

#define QUESTION_FILE "questions.txt"
#define TRANSLATION_FILE "translations.txt"

//选择题结构体，包含题干、四个选项、一个答案
struct Question {
    std::string question;
    std::vector<std::string> options;
    int answer;
    double essentiality;
};

//翻译题结构体，包含题干、答案
struct Translation {
    std::string question;
    std::string answer;
};

//全局缓存题库
std::vector<Question> questions;
std::vector<Translation> translations;

//全局文件流
std::fstream qFile;
std::fstream tFile;

//函数声明
void addQuestion();
void addTranslation();
void startQuestionQuiz();
void startTranslationQuiz();
void readQuestionsFromFile();
void readTranslationsFromFile();
void clearCinBuffer();
int getWeightedRandomQuestionIndex();

//清空cin缓冲区工具函数，解决getline吞换行
void clearCinBuffer() {
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

//从文件读取选择题到缓存
void readQuestionsFromFile() {
    questions.clear(); // 清空旧数据，防止重复加载
    qFile.close();
    qFile.open(QUESTION_FILE, std::ios::in);
    if (!qFile.is_open()) {
        std::cout << "选择题文件读取失败！" << std::endl;
        system("pause");
        qFile.open(QUESTION_FILE, std::ios::app | std::ios::in);
        return;
    }

    std::string line;
    while (std::getline(qFile, line)) {
        Question q;
        q.question = line;

        // 读取4个选项
        for (int i = 0; i < 4; i++) {
            if (!std::getline(qFile, line)) break;
            q.options.push_back(line);
        }

        // 读取答案数字
        if (!std::getline(qFile, line)) break;
        try {
            q.answer = std::stoi(line);
        } catch (...) {
            continue;
        }

        //重要性默认为1.0
        q.essentiality = 1.0;

        questions.push_back(q);
    }
    qFile.close();
    qFile.open(QUESTION_FILE, std::ios::app | std::ios::in); // 切回追加读写模式
}

//从文件读取翻译题到缓存
void readTranslationsFromFile() {
    translations.clear();
    tFile.close();
    tFile.open(TRANSLATION_FILE, std::ios::in);
    if (!tFile.is_open()) {
        std::cout << "翻译题文件读取失败！" << std::endl;
        system("pause");
        tFile.open(TRANSLATION_FILE, std::ios::app | std::ios::in);
        return;
    }

    std::string line1, line2;
    while (std::getline(tFile, line1)) {
        if (!std::getline(tFile, line2)) break;
        Translation t;
        t.question = line1;
        t.answer = line2;
        translations.push_back(t);
    }
    tFile.close();
    tFile.open(TRANSLATION_FILE, std::ios::app | std::ios::in);
}

//添加选择题
void addQuestion() {
    system("cls");
    std::string question;
    std::vector<std::string> options;
    int answer;

    clearCinBuffer();
    std::cout << "===== 添加选择题 =====" << std::endl;
    std::cout << "请输入选择题题干：" << std::endl;
    std::getline(std::cin, question);

    for (int i = 0; i < 4; i++) {
        std::string opt;
        std::cout << "请输入选项" << i + 1 << "：";
        std::getline(std::cin, opt);
        options.push_back(opt);
    }

    std::cout << "请输入正确答案(1~4)：";
    std::cin >> answer;

    // 写入文件
    qFile << question << std::endl;
    for (auto& s : options) {
        qFile << s << std::endl;
    }
    qFile << answer << std::endl;
    qFile.flush();

    std::cout << "\n✅ 选择题添加成功！按任意键返回菜单" << std::endl;
    clearCinBuffer();
    system("pause");
}

//添加翻译题
void addTranslation() {
    system("cls");
    std::string q, a;
    clearCinBuffer();
    std::cout << "===== 添加翻译题 =====" << std::endl;
    std::cout << "请输入题目：";
    std::getline(std::cin, q);
    std::cout << "请输入标准答案：";
    std::getline(std::cin, a);

    tFile << q << std::endl;
    tFile << a << std::endl;
    tFile.flush();

    std::cout << "\n✅ 翻译题添加成功！按任意键返回菜单" << std::endl;
    clearCinBuffer();
    system("pause");
}

//选择题刷题
void startQuestionQuiz() {
    system("cls");
    readQuestionsFromFile();
    if (questions.empty()) {
        std::cout << "❌ 暂无选择题，请先添加题目！" << std::endl;
        system("pause");
        return;
    }
    srand((unsigned)time(nullptr)); // 初始化随机种子

    while (true) {
        // 加权随机获取下标，替换原纯随机 rand() % questions.size()
        int idx = getWeightedRandomQuestionIndex();
        Question q = questions[idx];
        std::cout << "=====================" << std::endl;
        std::cout << q.question << std::endl;
        for (int i = 0; i < 4; i++) {
            std::cout << i + 1 << ". " << q.options[i] << std::endl;
        }
        std::cout << "=====================" << std::endl;
        std::cout << "输入答案(1-4)，输入0退出刷题：";
        int userAns;
        std::cin >> userAns;

        if (userAns == 0) break;

        if (userAns == q.answer) {
            std::cout << "✅ 回答正确！" << std::endl;
            q.essentiality *= 0.9; // 降低重要性
        } else {
            std::cout << "❌ 回答错误，正确答案：" << q.answer
                      << ". " << q.options[q.answer - 1] << std::endl;
            q.essentiality *= 1.1; // 提高重要性
        }
        std::cout << "\n按任意键下一题...";
        clearCinBuffer();
        system("pause");
        system("cls");
    }
}

//翻译刷题
void startTranslationQuiz() {
    system("cls");
    readTranslationsFromFile();
    if (translations.empty()) {
        std::cout << "❌ 暂无翻译题，请先添加题目！" << std::endl;
        system("pause");
        return;
    }
    srand((unsigned)time(nullptr));

    while (true) {
        int idx = rand() % translations.size();
        Translation t = translations[idx];
        std::cout << "=====================" << std::endl;
        std::cout << "题目：" << t.question << std::endl;
        std::cout << "请写出翻译答案(输入0退出刷题)(每次进入系统第一次输入无需换行，之后需要换行输入)：";
        clearCinBuffer();
        std::string userAns;
        std::getline(std::cin, userAns);

        if (userAns == "0") break;

        if (userAns == t.answer) {
            std::cout << "✅ 翻译正确！" << std::endl;
        } else {
            std::cout << "❌ 翻译错误，标准答案：" << t.answer << std::endl;
        }
        std::cout << "\n按任意键下一题...";
        system("pause");
        system("cls");
    }
}

// 加权随机抽取下标：essentiality越大，抽到概率越高
int getWeightedRandomQuestionIndex() {
    if (questions.empty()) return -1;
    // 计算所有权重总和
    double totalWeight = std::accumulate(
        questions.begin(), questions.end(), 0.0,
        [](double sum, const Question& q) { return sum + q.essentiality; }
    );
    // 生成 [0, totalWeight) 随机浮点数
    double randVal = (double)rand() / RAND_MAX * totalWeight;
    double curSum = 0.0;
    // 遍历找到命中区间
    for (int i = 0; i < questions.size(); i++) {
        curSum += questions[i].essentiality;
        if (randVal < curSum) {
            return i;
        }
    }
    return questions.size() - 1; // 兜底返回最后一题
}

//主菜单
int mainMenu() {
    system("cls");
    std::cout << "========== 原神英语刷题系统 ==========" << std::endl;
    std::cout << "1. 添加选择题" << std::endl;
    std::cout << "2. 添加翻译题" << std::endl;
    std::cout << "3. 选择题刷题" << std::endl;
    std::cout << "4. 翻译题刷题" << std::endl;
    std::cout << "5. 退出系统" << std::endl;
    std::cout << "======================================" << std::endl;
    std::cout << "请输入操作序号：";

    int op;
    std::cin >> op;
    switch (op) {
        case 1:
            addQuestion();
            break;
        case 2:
            addTranslation();
            break;
        case 3:
            startQuestionQuiz();
            break;
        case 4:
            startTranslationQuiz();
            break;
        case 5:
            std::cout << "👋 感谢使用，再见！" << std::endl;
            return 0;
        default:
            std::cout << "⚠️ 输入无效，请按任意键重新选择！" << std::endl;
            clearCinBuffer();
            system("pause");
            break;
    }
    return 1;
}

int main() {
    // 设置控制台UTF8编码，中文不乱码
    system("chcp 65001 >nul");

    // 打开文件：读写+追加，不存在自动创建
    qFile.open(QUESTION_FILE, std::ios::app | std::ios::in);
    tFile.open(TRANSLATION_FILE, std::ios::app | std::ios::in);
    if (!qFile.is_open() || !tFile.is_open()) {
        std::cout << "文件打开失败，程序退出！" << std::endl;
        system("pause");
        return 1;
    }

    // 主菜单循环
    while (mainMenu() != 0);

    // 关闭文件流
    qFile.close();
    tFile.close();
    return 0;
}