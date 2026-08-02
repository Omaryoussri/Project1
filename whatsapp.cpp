#include <iostream>
#include <vector>
#include <string>
#include <ctime>
using namespace std;

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
            if(users[i].getCurrentUsername==username){
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
    }}

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
        return;}
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