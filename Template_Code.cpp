#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <fstream>
using namespace std;

// ========================
//        USER CLASS
// ========================
class User {
private:
    string username;
    string password;
    string phoneNumber;
    string status;
    string lastSeen;

public:
    User() {
        username = "";
        password = "";
        phoneNumber = "";
        status = "";

        time_t lastSeentime = time(nullptr);
        struct tm *localTime = localtime(&lastSeentime);
        char timeDisplayFormat[80];
        strftime(timeDisplayFormat, 80, "%Y/%m/%d %I:%M:%S %p", localTime);
        lastSeen = timeDisplayFormat;
    }

    User(string uname, string pwd, string phone) {
        username = uname;
        password = pwd;
        phoneNumber = phone;
        updateLastSeen();
    }

    string getUsername() const {
        return username;
    }

    string getPhoneNumber() const {
        return phoneNumber;
    }

    string getStatus() const {
        return status;
    }

    string getLastSeen() const {
        return lastSeen;
    }

    void setStatus(string newStatus) {
        status = newStatus;
    }

    void setPhoneNumber(string phone) {
        phoneNumber = phone;
    }

    void updateLastSeen() {
        time_t lastSeentime = time(nullptr);
        struct tm* localTime = localtime(&lastSeentime);
        char timeDisplayFormat[80];
        strftime(timeDisplayFormat, 80, "%Y/%m/%d %I:%M:%S %p", localTime);
        lastSeen = timeDisplayFormat;
    }

    bool checkPassword(string pwd) const {
        if(pwd == password){
            return true;
        }
        return false;
    }

    void changePassword(string newPwd) {
        if (newPwd.length() < 6) {
            cout << "Password too short\n";
        }
        else {
            password = newPwd;
        }
    }
};

// ========================
//      MESSAGE CLASS
// ========================
class Message {
private:
    string sender;
    string content;
    string timestamp;
    string status;
    Message* replyTo;

public:
    Message() {
        sender = "";
        content= "";
        timestamp="";
        status="";
        replyTo=nullptr;
        updateTimestamp();
    }

    Message(string sndr, string cntnt) {
        sender=sndr;
        content=cntnt;
        status="sent";
        replyTo=nullptr;
        updateTimestamp();
    }

    string getContent() const {
        return content;
    }

    string getSender() const {
        return sender;
    }

    string getTimestamp() const {
        return timestamp;
    }

    string getStatus() const {
        return status;
    }

    Message* getReplyTo() const {
        return replyTo;
    }

    void setStatus(string newStatus) {
        status=newStatus;
    }

    void setReplyTo(Message* msg) {
        replyTo=msg;
    }

    void updateTimestamp() {
        time_t currentTime = time(nullptr);
        struct tm *localTime = localtime(&currentTime);
        char timeDisplayFormat[80];
        strftime(timeDisplayFormat, 80, "%Y/%m/%d %I:%M:%S %p", localTime);
        timestamp = timeDisplayFormat;
    }

    void display() const {
        cout << "(" << timestamp << ") " << sender << ":" << endl;
        if (replyTo != nullptr)
        {
            cout << "Replying to: " << replyTo->getContent() << endl;
        }
        
        cout << "Message: " << content << endl;
        cout << "Status: " << status << endl;
    }

    void addEmoji(string emojiCode) {
        string symbol = "";
        if (emojiCode ==":)") symbol = "😊";
        else if (emojiCode == ":(") symbol = "🙁";
        else if (emojiCode == ":D") symbol = "😃";
        else if (emojiCode == "<3") symbol = "❤️";
        else if (emojiCode == ":thumbsup:") symbol = "👍";

        int pos = content.find(emojiCode);
        if (pos != -1)
        {
            content.replace(pos, emojiCode.length(), symbol);
        }
    }
};

// ========================
//       CHAT CLASS (BASE)
// ========================
class Chat
{
protected:
    vector<string> participants;
    vector<Message> messages;
    string chatName;

public:
    Chat()
    {
        participants.clear();
        messages.clear();
        chatName = "";
    }

    Chat(vector<string> users, string name)
    {
        participants = users;
        chatName = name;
    }

    virtual ~Chat() = default;

    void addMessage(const Message& msg)
    {
        messages.push_back(msg);
    }

    bool deleteMessage(int index, const string& username)
    {
        if (index < 0 || index >= messages.size())
        {
            return false;
        }

        if (messages[index].getSender() != username)
        {
            return false;
        }

        messages.erase(messages.begin() + index);
        return true;
    }

    virtual void displayChat() const
    {
        cout << "Chat Name: " << chatName << endl;

        if (messages.empty())
        {
            cout << "No messages in this chat." << endl;
        }

        for (const Message& msg : messages)
        {
            msg.display();
        }
    }

    vector<Message> searchMessages(string keyword) const
    {
        vector<Message> results;

        for (const Message& msg : messages)
        {
            if (msg.getContent().find(keyword) != string::npos)
            {
                results.push_back(msg);
            }
        }
        return results;
    }

    void exportToFile(const string& filename) const
    {
        ofstream file(filename);
        if (!file.is_open())
        {
            cout << "Could not open file.\n";
            return;
        }

        file << "Chat: " << chatName << '\n';

        for (const Message& msg : messages)
        {
            file << msg.getSender()
                << " [" << msg.getTimestamp() << "]: "
                << msg.getContent() << '\n';
        }
    }
};

// ========================
//     PRIVATE CHAT CLASS
// ========================
class PrivateChat : public Chat {
private:
    string user1;
    string user2;

public:
    PrivateChat(string u1, string u2) {
        user1 = u1;
        user2 = u2;
        participants = {u1, u2};
        chatName = "Chat between " + u1 + " and " + u2;
    }

    void displayChat() const override {
        cout << chatName << endl;
        for (const Message& message : messages) {
            message.display(); 
        }
    }

    void showTypingIndicator(const string& username) const {
        cout << username << " is typing..." << endl;
    }
};

// ========================
//      GROUP CHAT CLASS
// ========================
class GroupChat : public Chat {
private:
    vector<string> admins;
    string description;

public:
    GroupChat(vector<string> users, string name, string creator)
        : Chat(users, name)
    {
        if (!isParticipant(creator)) 
            participants.push_back(creator);
        admins.push_back(creator); 
    }

    void addAdmin(string newAdmin) {
        admins.push_back(newAdmin);
    }

    bool removeParticipant(const string& admin, const string& userToRemove) {
        if (isAdmin(admin))
        {
            for (int i=0; i<participants.size(); i++){
                if (participants[i] == userToRemove){
                    participants.erase(participants.begin()+i);
                    return true;
                }
            }
        }
        return false;
    }

    bool isAdmin(string username) const {
        for (int i=0; i<admins.size(); i++){
            if (username == admins[i]) {
                    return true;
            }
        }
        return false;
    }

    bool isParticipant(string username) const {
        for (int i=0; i<participants.size(); i++){
            if (username == participants[i]) {
                    return true;
            }
        }
        return false;
    }

    void setDescription(string desc) {
        description = desc;
    }

    void displayChat() const override {
        cout << "Group Name: " << chatName << endl;
        if (!description.empty()) {
            cout << "Description: " << description << endl;
        }
        
        if (messages.empty()) {
            cout << "No messages in this group yet." << endl;
        }

        for (const Message& msg : messages) {
            msg.display();
        }
    }

    void sendJoinRequest(const string& username) {
        Message request(username, "The user " + username + " request to join");
        addMessage(request);
    }
};

// ========================
//    WHATSAPP APP CLASS
// ========================
class WhatsApp {
private:
    vector<User> users;
    vector<Chat*> chats;
    int currentUserIndex;

    int findUserIndex(string username) const {
        for(int i=0;i<(int)users.size();i++){
            if(users[i].getUsername()==username){
                return i;
            }
        }
        return -1;
    }

    bool isLoggedIn() const {
       if(currentUserIndex!=-1){
        return true;
       }
       else{
        return false;
       }
    }

    string getCurrentUsername() const {
        if (!isLoggedIn()) {
            return "";
        }
        return users[currentUserIndex].getUsername();
    }

public:
    WhatsApp() : currentUserIndex(-1) {}

    void signUp() {
        string user_name,pd,phone;
        cout << "\n--- Sign Up ---\n";
        cout << "Choose a username: ";
        cin.ignore();
        getline(cin,user_name);
        if (findUserIndex(user_name) != -1) {
           cout << "That username is already taken. Try again!\n";
           return;
        }
        cout << "Choose a password : ";
        cin>>pd;
        if (pd.length() < 6) {
            cout << "Password too short. Must be at least 6 characters!\n";
            return;
        }
        cout << "Enter phone number: ";
        cin>>phone;
        users.push_back(User(user_name, pd, phone));
        cout << "Account created successfully. You can now log in.\n";
    }

    void login() {
        string user_name,pd;
        cout << "\n--- Login ---\n";
        cout << "Username: ";
        cin.ignore();
        getline(cin,user_name);
        cout << "Password: ";
        cin >> pd;
        int index = findUserIndex(user_name);
        if (index == -1) {
            cout << "No account found with that username!\n";
            return;
        }

        if (!users[index].checkPassword(pd)) {
            cout << "Incorrect password!\n";
            return;
        }
        currentUserIndex = index;
        cout << "Welcome back, " << user_name << "!\n";
    }

    void startPrivateChat() {
       if (!isLoggedIn()) {
        cout << "You must be logged in first.\n";
        return;
       }
       string otherUser;
       cout << "\nEnter the username to chat with: ";
       cin.ignore();
       getline(cin,otherUser);
       if (findUserIndex(otherUser) == -1) {
           cout << "That user does not exist.\n";
           return;
       }
       string myuser = getCurrentUsername();
       if (otherUser == myuser) {
           cout << "You cannot start a chat with yourself.\n";
           return;
       }
       Chat* newChat = new PrivateChat(myuser, otherUser);
       chats.push_back(newChat);
       cout << "Private chat started with " << otherUser << ".\n";
    }

    void createGroup() {
       if (!isLoggedIn()) {
        cout << "You must be logged in to create a group.\n";
        return;
       }
       string groupName;
       cout << "\nEnter group name: ";
       cin.ignore();
       getline(cin, groupName);
       int numMembers;
       cout << "Enter how many numbers of member need to add:  ";
       cin >> numMembers;
       if (numMembers < 1) {
           cout << "A group needs at least 2 participants total.\n";
           return;
       }
       string myuser = getCurrentUsername();
       vector<string> participants;
       participants.push_back(myuser);
       for (int i = 0; i < numMembers; i++) {
           string member;
           cout << "Enter username of member " << (i + 1) << ": ";
           cin >> member;

           if (findUserIndex(member) == -1) {
               cout << "  -> User \"" << member << "\" not found, skip.\n";
               continue;
           }
           if (member == myuser) {
               cout << "  -> You're already in the group, skip.\n";
               continue;
           }
           participants.push_back(member);
       }
       if (participants.size() < 2) {
           cout << "Group creation failed: not enough participants!\n";
           return;
       }
       Chat* newGroup = new GroupChat(participants, groupName, myuser);
       chats.push_back(newGroup);
       cout << "Group \"" << groupName << "\" created with "
            << participants.size() << " members.\n";
    }

    void viewChats() const {
        if (!isLoggedIn()) {
            cout << "You must be logged in to view chats.\n";
            return;
        }
        string myuser = getCurrentUsername();
        bool found = false;
        cout << "\n--- Your Chats ---\n";
        for (Chat* c : chats) {
            PrivateChat* pc = dynamic_cast<PrivateChat*>(c);
            GroupChat* gc = dynamic_cast<GroupChat*>(c);
            bool belongsHere = false;
            if (pc != nullptr) {
                belongsHere = true;
            } else if (gc != nullptr) {
                belongsHere = gc->isParticipant(myuser);
            }
            if (belongsHere) {
                c->displayChat();
                found = true;
            }
        }
        if (!found) {
            cout << "You have no chats yet.\n";
        }
    }

    void logout() {
       if (!isLoggedIn()) {
        cout << "You are not logged in.\n";
        return;
       }
       users[currentUserIndex].updateLastSeen();
       cout << "Goodbye, " << getCurrentUsername() << "!\n";
       currentUserIndex = -1;
    }

    void run() {
        while (true) {
            if (!isLoggedIn()) {
                cout << "\n1. Login\n2. Sign Up\n3. Exit\nChoice: ";
                int choice;
                cin >> choice;

                if (choice == 1) login();
                else if (choice == 2) signUp();
                else if (choice == 3) break;
            }
            else {
                cout << "\n1. Start Private Chat\n2. Create Group\n3. View Chats\n4. Logout\nChoice: ";
                int choice;
                cin >> choice;

                if (choice == 1) startPrivateChat();
                else if (choice == 2) createGroup();
                else if (choice == 3) viewChats();
                else if (choice == 4) logout();
            }
        }
    }
};

// ========================
//          MAIN
// ========================
int main() {
    WhatsApp whatsapp;
    whatsapp.run();
    return 0;
}