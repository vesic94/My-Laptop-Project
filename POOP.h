#include <iostream>
#include <cstdlib>
#include <map>
#include <unordered_map>
#include <string>
#include <iterator>
#include <fstream>
#include <ostream>
#include <vector>
#include <queue>
#include <ctime>
#include <stack>
#include <regex>
#include <stdio.h>

using namespace std;

int bookid = 0, personid = 0;// , publisherIDD = 0;

class NoFile{};
inline ostream& operator<<(ostream &it, const NoFile *e)
{
	it << "Ne postoji datoeka sa takvim imenom" << endl;
	return it;
}
class BadNumber{};
inline ostream& operator<<(ostream &it, const BadNumber *b)
{
	it << "Uneli ste los broj" << endl;
	return it;
}

class Member
{
	int memID;
public:
	Member(int mem) :memID(mem){}
	void print(ostream &it) const { it << "Member ID:" << memID; }
	const int myMemID()const { return memID; }
};

class Person{
	int personID = ++personid; //licna karta
	string ime, prezime;
	Member *member;
public:
	int myID() { return personID; }
	friend ostream& operator<<(ostream &it, const Person *p)
	{
		p->print(it); //Odgovor na zamerku, ideja: Mozda u buducnosti bude potrebno modeliranje Membera kao potklase klase Person, ostavka mogucnosti za polimorfno ispisivanje;
		return it;
	}
	void print(ostream &it)const { it << ime << " " << prezime << " " << " ID:" << personID; if (member != nullptr) member->print(it); }
	Person(string im, string prez) :ime(im), prezime(prez), member(nullptr){}
	void setMemID(int mem){ member = new Member(mem); }
	const int myMemID()const
	{
		if (member != nullptr) return member->myMemID();
		else return -1;
	}
};

class SetOfPublishers :unordered_map<int, string>
{
	friend class Book;
	int idpub = 0;
public:
	int isthere(string str){
		for (unordered_map<int, string>::iterator it = begin(); it != end(); ++it)
		{
			if (it->second == str) return it->first;
		}
		return 0;
	}
	int addPublisher(string publisher)
	{
		int publisherID = ++idpub;
		unordered_map<int, string>::iterator it = unordered_map<int, string>::find(publisherID);
		if (it != unordered_map<int, string>::end()) return it->first;
		unordered_map<int, string>::emplace(publisherID, publisher);
		return publisherID;
	}
	string find(int id)
	{
		unordered_map<int, string>::iterator it = unordered_map<int, string>::find(id);
		if (it == unordered_map<int, string>::end()) return "";
		return it->second;
	}
	void removePublisher(int id)
	{
		unordered_map<int, string>::erase(id);
	}
};
SetOfPublishers set;

class Date{
	int yy, mm, dd;
public:
	Date(const Date &&d){ yy = d.yy; mm = d.mm; dd = d.dd; };
	Date(int y, int m, int d) :yy(y), mm(m), dd(d){}
	friend ostream & operator<<(ostream &it, const Date *d)
	{
		it << d->dd << "." << d->mm << "." << d->yy;
		return it;
	}
	friend bool operator>(Date &d1, Date &d2)
	{
		return (d1.yy > d2.yy || (d1.yy == d2.yy && d1.mm > d2.mm) || (d1.yy == d2.yy && d1.mm == d2.mm && d1.dd >= d2.dd));
	}
	friend bool operator<(Date &d1, Date &d2)
	{
		return !(d1 == d2 || d1>d2);
	}
	friend bool operator==(Date &d1, Date &d2)
	{
		return (d1.yy == d2.yy && d1.mm == d2.mm && d1.dd == d2.dd);
	}
	friend bool operator<=(Date &d1, Date &d2)
	{
		return(d1 < d2 || d1 == d2);
	}
	friend bool operator>=(Date &d1, Date&d2)
	{
		return (d1>d2 || d1 == d2);
	}
};

class BorrowingPosition{
public:
	friend ostream& operator<<(ostream &it, const BorrowingPosition *p)
	{
		p->pisi(it);
		return it;
	}
	virtual void pisi(ostream &it) const = 0;
	virtual bool isBookInLibrary()const = 0;
	virtual ~BorrowingPosition(){};
};

class Position :public BorrowingPosition{
	int shelfID, cabinetID, roomID;
public:
	virtual bool isBookInLibrary()const override
	{
		return 1;
	}
	Position(int shelf, int cabinet, int room) :shelfID(shelf), cabinetID(cabinet), roomID(room){}

	void pisi(ostream &it)const override{
		it << "Knjiga je u prostoriji:" << roomID << ", U ormaru:" << cabinetID << ", Na polici:" << shelfID << endl;
	}
	~Position(){};
};

void setJoker(string &reg)
{
	reg += '(';
	reg += '.';
	reg += '*';
	reg += ')';
}

void joker_End(string &reg, string &given)
{
	reg += given;
	reg.resize(reg.length() - 1);
	setJoker(reg);
}
void joker_Beginning(string &reg, string &given)
{
	setJoker(reg);
	for (int i = 1; i < given.length(); i++)
		reg += given[i];
}

bool check_regex(string &checker, string checking)
{
	string checker_extracted = "";
	if (checker[checker.length() - 1] == '$') joker_End(checker_extracted, checker);
	else
	if (checker[0] == '$') joker_Beginning(checker_extracted, checker);
	else
	for (int i = 0; i < checker.length(); i++)
	{
		if (checker[i] == '$') { setJoker(checker_extracted); i++; }
		checker_extracted += checker[i];
	}
	regex rule(checker_extracted);
	if (regex_match(checking, rule)) return true;
	else return false;
}

class Autori{
	int brAutora;
	string *imena, *prezimena;
public:
	bool any(string autor)
	{
		for (int i = 0; i < brAutora; i++)
			if (check_regex(autor, imena[i])) return true;

		return false;
	}
	friend ostream& operator<<(ostream &it, const Autori *a)
	{
		for (int i = 0; i < a->brAutora; i++){
			it << a->imena[i] << " " << a->prezimena[i];
			if (a->brAutora - 1 != i) it << ", ";
			else it << ". ";
		}
		return it;
	}
	Autori(int br, string *im, string *pr) :brAutora(br), imena(im), prezimena(pr){}
	friend bool operator==(const Autori &a1, const Autori &a2)
	{
		bool flag = false;
		if (a1.brAutora == a2.brAutora)
		{
			flag = true;
			for (int i = 0; i < a1.brAutora; i++)
			{
				if (a1.imena[i] != a2.imena[i] || a1.prezimena[i] != a2.prezimena[i]) { flag = false; break; }
			}
		}
		return flag;
	}
};

class ISBN
{
	string isbn10;
	string isbn13;
	bool tip13;
public:
	bool isbn_13()const { return tip13; }
	ISBN(bool t, string isbn) :tip13(t){
		if (tip13)
			isbn13 = isbn;
		else
			isbn10 = isbn;
	}
	string getISBN()
	{
		if (tip13)
			return isbn13;
		else
			return isbn10;
	}
	friend ostream& operator<<(ostream &it, const ISBN &isbn)
	{
		if (isbn.tip13)
			it << isbn.isbn13;
		else
			it << isbn.isbn10;
	}
};

enum BookCondition{ NOVA, MALO_OSTECENA, VEOMA_OSTECENA, POTREBNO_ZAMENITI, UKLONJENA, NIJEBILA };
static string niz[6] = { "NOVA", "MALO OSTECENA", "VEOMA OSTECENA", "POTREBNO ZAMENITI", "UKLONJENA", "VAN BIBLIOTEKE" };
static BookCondition nizz[6] = { NOVA, MALO_OSTECENA, VEOMA_OSTECENA, POTREBNO_ZAMENITI, UKLONJENA, NIJEBILA };
class Book{
	int brojIzdanja;
	BookCondition condition;
	BorrowingPosition *p;
	string naziv;
	int nazivIzdavaca;
	int bookID;
	Autori *autor;
	int wrapID = -1;
	int godizd;
	ISBN *isbn; //dodaj u konstruktor;
	string zanr; // dodaj
	string jezik; //dodaj
public:
	friend bool operator==(const Book &b1, const Book &b2)
	{
		return (b1.bookID == b2.bookID);
	}
	string getZanr()const { return zanr; }
	string getLang()const { return jezik; }

	Autori* getAutor()const { return autor; }
	void addWrapper(int id){ wrapID = id; }
	int readWrapper()const { return wrapID; }
	~Book()
	{
		p = nullptr;
		autor = nullptr;
	}
	Book(BookCondition bc, BorrowingPosition *pp, string naz, int godinaizd, Autori *aut, string nazivIzd, int podatakOIZD, string isbnn, bool tip, string zanrr, string jezikk) :
		p(pp), naziv(naz), godizd(godinaizd), condition(bc), autor(aut), brojIzdanja(podatakOIZD), jezik(jezikk), zanr(zanrr){
		nazivIzdavaca = set.addPublisher(nazivIzd);
		isbn = new ISBN(tip, isbnn);
	}
	void addBookID(int bkid){
		bookID = bkid;
	}
	void easyPrint(){
		cout << naziv << "," << set.find(nazivIzdavaca) << "-" << autor << "Godina Izdanja " << godizd << endl;
	}
	void pisi(ostream &it)const{
		it << naziv << "-" << autor << " Izdao: " << set.find(nazivIzdavaca) << ". " << endl;
		it << "Stanje: " << niz[condition] << "(" << godizd << ") " << "broj Izdanja: " << brojIzdanja << " ID: " << bookID << endl;
		it << p << endl;
	}
	friend ostream& operator<<(ostream &it, const Book *knjiga){
		knjiga->pisi(it);
		return it;
	}
	BorrowingPosition *location(){
		return p;
	}
	void position()
	{
		if (p != nullptr) cout << p;
	}
	void taken_position(BorrowingPosition *bp)	{ p = bp; }
	int getYear()const{ return godizd; }
	//const int getPublishersID() const {return SetOfPublishers::find(nazivIzdavaca);}
	const string getBooksName()const{ return naziv; }
	const string getPublishersName() const{ return set.find(nazivIzdavaca); }
	const int getBookID()const{ return bookID; }
	int getCondition()const
	{
		return condition;
	}
	void setCondition(BookCondition c){ condition = c; }
};

class Borrowing :public BorrowingPosition{
	Book *knjiga;
	Date *begin, *end;
	bool back = 0;
	int memberID;
	BookCondition stanje;
public:
	bool same_Borrow(int memID, Book *knji, Date *beg, Date *en)
	{
		return (memID == memberID && *knji == *knjiga && *beg == *begin && *en == *end);
	}
	~Borrowing(){ end = begin = nullptr; knjiga = nullptr; }
	virtual bool isBookInLibrary() const override
	{
		return 0;
	}
	Borrowing(Book *knj, Date *beg, Date *end, int memID) :
		begin(beg), memberID(memID), end(end), back(0), knjiga(knj){
		stanje = (BookCondition)knj->getCondition();
	}
	const void vratio(BookCondition bcd){ if (stanje > bcd) cout << "Lose Ste Vodili Racuna o knjizi!!!" << endl; back = 1; }
	Date *datumVracanja(){
		return end;
	}
	Date * uzeta(){
		return begin;
	}
	bool vracena(){ return back; }
	Book* book(){ return knjiga; } //pok
	int memID()const { return memberID; }
	void print(ostream &it)const{
		if (knjiga)
		{
			it << "Knjiga:" << knjiga;
			//pisi(it);
		}
	}
	void pisi(ostream &it) const override{
		it << "  Od: " << begin << "  Do: " << end; //a->readMember(memberID) << "  Od: " << begin << "  Do: " << end;
		if (!back) it << endl << "  ****NIJE VRACENA****  " << endl;
	}
	friend ostream&operator<<(ostream &it, const Borrowing b)
	{
		b.print(it);
		return it;
	}
};

static const int T = 100;

class Result
{
	Book **knjige;
	int kap, num;
public:
	~Result(){ 
		for (int i = 0; i < num; i++)
			knjige[i] = nullptr;
		delete knjige;
		kap = num = 0;
	}

	void more()
	{
		int kapNew = 2 * kap;
		Book **knjigeNew = new Book*[kapNew];
		for (int i = 0; i < num; i++)
		{
			knjigeNew[i] = knjige[i];
			knjige[i] = nullptr;
		}
		delete knjige;
		knjige = knjigeNew;
		kap = kapNew;
	}
	Result() :kap(10), num(0)
	{
		knjige = new Book*[10];
	}
	void operator+=(Book *knj)
	{
		if (num == kap) more();
		knjige[num++] = knj;
	}
	void operator+=(Result *r)
	{
		for (int i = 0; i < r->getNum(); i++)
			if (!in((*r)[i]))	this->operator+=((*r)[i]);
	}
	bool in(Book *b1)
	{
		for (int i = 0; i < num; i++)
			if (*b1 == *knjige[i]) return true;

		return false;
	}

	int getNum()const { return num; }
	friend ostream& operator<<(ostream &it, const Result *r)
	{
		for (int i = 0; i < r->getNum(); i++)
		{
			it << r->knjige[i];
		}
		return it;
	}
	void dismiss(int k)
	{
		if (num>0)
		for (int i = k; i < num-1; i++)
		{
			knjige[i] = knjige[i + 1];
		}
		num--;
	}
	Book* operator[](int i)
	{
		if (i < num)
			return knjige[i];
		else return nullptr;
	}
	bool is_empty()const { return num == 0; }
	Book** getAllBooks(){ return knjige; }
};



class BookCollection{
	int wrapperID = 0;
	class BookWrapper{
		/*struct cmpclass{
		bool operator()(const Book &levi, const Book &desni){
		return levi.getCondition() < desni.getCondition();
		}*/
		//};
		//priority_queue<Book*,deque<Book*>, cmpclass> sameBooks;
		int myID, num;
		//string name;
		Book *vector[T];
	public:
		void anyPublication(int year, Result *rez)
		{
			for (int i = 0; i < num; i++)
			{
				if (vector[i]->getYear() == year) *rez += vector[i];
			}
		}
		bool IDin(int bookID)const {
			for (int i = 0; i < num; i++)
			{
				if (vector[i]->getBookID() == bookID) return true;
			}
			return false;
		}
		Book* getBook(int bookID){
			for (int i = 0; i < num; i++)
			{
				if (vector[i]->getBookID() == bookID) return vector[i];
			}
			return nullptr;
		}
		string wrapperName()const { return vector[0]->getBooksName(); }
		Autori* wrapperAutors() const { return vector[0]->getAutor(); }
		string wrapperPublisher()const { return vector[0]->getPublishersName(); }
		int zamena()
		{
			int t = 0;
			for (int i = 0; i < num; i++)
			{
				if (vector[i]->getCondition() == POTREBNO_ZAMENITI) t++;
			}
			return t;
		}
		int pozajmljene()
		{
			int t = 0;
			for (int i = 0; i < num; i++)
			{
				if (!vector[i]->location()->isBookInLibrary()) t++; //knjige u vlasnistvu clanova
			}
			return t;
		}
		int numOfBooks(){
			int t = 0;
			for (int i = 0; i < num; i++)
			{
				if (vector[i]->location()->isBookInLibrary()) t++; //knjige trenutno u biblioteci(U posedu biblioteke)
			}
			return t;
		}
		int myIDD()const { return myID; } //mmm?
		friend ostream& operator<<(ostream &it, const BookWrapper *bw)
		{
			for (int i = 0; i < bw->num; i++)
			{
				it << bw->vector[i];
			}
			return it;
		}
		Book* getBook()
		{
			if (!num) return nullptr;
			int min = 0;
			for (int i = 0; i < num; i++)
			{
				if (vector[i]->getCondition() < vector[min]->getCondition() && vector[i]->getCondition() != UKLONJENA || vector[min]->getCondition() == UKLONJENA)
					min = i;
			}
			Book *get = vector[min];
			if (get->getCondition() == UKLONJENA) return nullptr;
			for (int i = min; i < num - 1; i++)
			{
				vector[i] = vector[i + 1];
			}
			return get;

		}
		void addBook(Book *knjiga)
		{
			if (num >= T) return;
			vector[num++] = knjiga;
		}
		//~BookWrapper(){
		//	delete vector;
		//}
		BookWrapper(Book *first) :num(1)
		{
			vector[0] = first;
			myID = first->readWrapper();
		}
		bool check(Book *knjiga)
		{
			return vector[0]->getAutor() == knjiga->getAutor() && vector[0]->getBooksName() == knjiga->getPublishersName() && vector[0]->getPublishersName() == knjiga->getPublishersName() && vector[0]->getYear() == knjiga->getYear();
		}
		void removeBook(int bookID){
			Book* b = nullptr;
			for (int i = 0; i < num; i++)
			{
				if (vector[i]->getBookID() == bookID) {
					b = vector[i];
					break;
				}
			}
			if (b) b->setCondition(UKLONJENA);
		}
		void remove(int index)
		{
			for (int i = index; i < num - 1; i++)
				vector[i] = vector[i + 1];
			num--;
		}
		void removeBook_Completely(int bookID)
		{
			int ind = -1;
			for (int i = 0; i < num; i++)
			{
				if (vector[i]->getBookID() == bookID) {
					ind = i;
					break;
				}
			}
			if (ind != -1) remove(ind);
		}
		Book* look(){
			return vector[num - 1];
		}
		void fillResult(Result *r)
		{
			for (int i = 0; i < num; i++)
			{
				*r += vector[i];
			}
		}
		void allDate(Date *d, Result *r)
		{
			for (int i = 0; i < num; i++)
			if (*d == *((Borrowing *)vector[i]->location())->datumVracanja()) *r += vector[i];
		}

		void allCondition(BookCondition c, Result *r){
			for (int i = 0; i < num; i++)
			if (c == vector[i]->getCondition()) *r += vector[i];
		}
		Autori* getAuthors(){
			return vector[0]->getAutor();
		}
	};
	map<int, BookWrapper*> collection;
public:
	~BookCollection(){
		for (map<int, BookWrapper*>::iterator it = collection.begin(); it != collection.end(); ++it)
			delete it->second;
	}
	void addBook(Book *knjiga)
	{
		bool bookPlaced = false;
		for (map<int, BookWrapper*>::iterator it = collection.begin(); it != collection.end(); ++it)
		{
			if (it->second != nullptr && it->second->check(knjiga)) {
				knjiga->addWrapper(it->second->myIDD());
				it->second->addBook(knjiga);
				bookPlaced = true;
				break;
			}
		}
		if (!bookPlaced) {
			knjiga->addWrapper(wrapperID++);
			collection.emplace(knjiga->readWrapper(), new BookWrapper(knjiga));
		}

	}
	int zamena(){
		int t = 0;
		for (map<int, BookWrapper*>::iterator it = collection.begin(); it != collection.end(); ++it)
			t += it->second->zamena();
		return t;
	}
	int pozajmljene(){
		int t = 0;
		for (map<int, BookWrapper*>::iterator it = collection.begin(); it != collection.end(); ++it)
			t += it->second->pozajmljene();
		return t;
	}
	int numOfBooks(){
		int t = 0;
		for (map<int, BookWrapper*>::iterator it = collection.begin(); it != collection.end(); ++it)
			t += it->second->numOfBooks();
		return t;
	}
	void removeBook(int bookID)
	{
		for (map<int, BookWrapper*>::iterator it = collection.begin(); it != collection.end(); ++it)
		if (it->second->IDin(bookID))	{
			it->second->removeBook(bookID);
			break;
		}
	}
	void removeBook_Completely(int bookID)
	{
		for (map<int, BookWrapper*>::iterator it = collection.begin(); it != collection.end(); ++it)
		if (it->second->IDin(bookID))	{
			it->second->removeBook_Completely(bookID);
			break;
		}
	}
	void printAllBooks(ostream &itok = cout)
	{
		for (map<int, BookWrapper*>::iterator it = collection.begin(); it != collection.end(); ++it)
			itok << it->second;
	}
	Book* lookAtBook(int bookID)
	{
		for (map<int, BookWrapper*>::iterator it = collection.begin(); it != collection.end(); ++it)
		{
			Book* knj = it->second->getBook(bookID);
			if (knj) return knj;
		}
		return nullptr;

	}
	Result* search_Name_Book(string naziv, string nazizd = "", int godina = 0){
		Result *result = new Result();
		for (map<int, BookWrapper*>::iterator it = collection.begin(); it != collection.end(); ++it)
		if (check_regex(naziv, it->second->look()->getBooksName()))//ovde
		{
			if (nazizd != "" && godina != 0){
				if (check_regex(nazizd, it->second->look()->getPublishersName()) && godina == it->second->look()->getYear())
					it->second->fillResult(result);
			}
			else it->second->fillResult(result);

		}
		return result;
	}
	Result * search_publication(int year)
	{
		Result* result = new Result();
		for (map<int, BookWrapper*>::iterator it = collection.begin(); it != collection.end(); ++it)
		{
			it->second->anyPublication(year, result);
		}
		return result;
	}


	Result* search_Author(string autor)
	{
		Result* result = new Result();
		for (map<int, BookWrapper*>::iterator it = collection.begin(); it != collection.end(); ++it)
		{
			if (it->second->getAuthors()->any(autor)) it->second->fillResult(result);
		}
		return result;
	}
	Result* search_Date(Date *d){
		Result *result = new Result();
		for (map<int, BookWrapper*>::iterator it = collection.begin(); it != collection.end(); ++it)
		if (!it->second->look()->location()->isBookInLibrary())
			it->second->allDate(d, result);
		return result;
	}
	Result* search_Condition(BookCondition c){
		Result *result = new Result();
		for (map<int, BookWrapper*>::iterator it = collection.begin(); it != collection.end(); ++it)
			it->second->allCondition(c, result);
		return result;
	}
};

class Library{
	static int N;
	class FilesInLibrary{
		Library *a;
		multimap<int, Borrowing *> pozajmio;
	public:
		~FilesInLibrary(){
			for (map<int, Borrowing*>::iterator it = pozajmio.begin(); it != pozajmio.end(); ++it)
				delete it->second; a = nullptr;
		}
		FilesInLibrary(Library *aa) :a(aa){}

		void izmedju(Date *begin, Date *end)
		{
			if (pozajmio.empty()) return;
			for (multimap<int, Borrowing*>::iterator it = pozajmio.begin(); it != pozajmio.end(); ++it)
			if (*begin <= *it->second->uzeta() && *end > *it->second->uzeta() || *end >= *it->second->datumVracanja() && *begin <= *it->second->datumVracanja() || *begin >= *it->second->uzeta() && *end <= *it->second->datumVracanja())
				cout << a->readMember(it->second->memID()) << endl;
		}
		void istekaoRok(Date *end)
		{
			bool ima = false;
			if (pozajmio.empty()) return;
			for (multimap<int, Borrowing*>::iterator it = pozajmio.begin(); it != pozajmio.end(); ++it)
			if (*it->second->datumVracanja() < *end && !it->second->vracena())
			{
				ima = true;
				cout << a->readMember(it->second->memID()) << endl;//*it->second; ;
			}
			if (!ima) cout << "Ni jedan clan nije prekoracio rok do danasnjeg dana." << endl;
		}
		void borrowings(int clanID){
			pair<multimap<int, Borrowing*>::iterator, multimap<int, Borrowing *>::iterator> ret;
			ret = pozajmio.equal_range(clanID);
			if (ret.first == pozajmio.end()) {
				cout << "Clan Jos Nije Pozajmio Nista" << endl; return;
			}
			cout << "clan: " << a->readMember(clanID) << "je pozjamio : " << endl;
			multimap<int, Borrowing *>::iterator it;
			for (it = ret.first; it != ret.second; ++it)
			{
				it->second->book()->easyPrint(); cout << it->second << endl;
			}
		}
		void undo_Borrowing(int memID, Book *knjiga, Date *beg, Date *end)
		{
			std::pair<std::multimap<int, Borrowing*>::iterator, std::multimap<int, Borrowing *>::iterator> ret = pozajmio.equal_range(memID);
			if (ret.first == pozajmio.end()) {
				return;//GRESKA PRIJAVI
			}
			std::multimap<int, Borrowing *>::iterator it;
			Borrowing *bb;
			for (it = ret.first; it != ret.second; ++it)
			{
				Borrowing *b = bb = it->second;
				if (it->second->same_Borrow(memID, knjiga, beg, end))// same?
				{
					delete it->second; it->second = nullptr;
					pozajmio.erase(memID);
					break;
				}
			}
		}
		void all(ostream &itok = cout){
			std::multimap<int, Borrowing *>::iterator it;
			for (it = pozajmio.begin(); it != pozajmio.end(); ++it)
			{
				itok << a->readMember(it->first) << ", " << it->second->book();
				if (it->second->vracena()) itok << it->second;
				itok << endl;
			}
		}
		void borrow_Book(int memID, Book *knjiga, Date *beg, Date *end){
			Borrowing *b = new Borrowing(knjiga, beg, end, memID);
			pozajmio.emplace(memID, b);
			knjiga->taken_position(b);
		}
		void return_Book(int memID, Book *knjiga, BookCondition bcd){
			std::pair<std::multimap<int, Borrowing*>::iterator, std::multimap<int, Borrowing *>::iterator> ret = pozajmio.equal_range(memID);
			if (ret.first == pozajmio.end()) {
				cout << "Clan Jos Nije Pozajmio nista" << endl; return;
			}
			std::multimap<int, Borrowing *>::iterator it;
			bool bookFound = false;
			Borrowing *bb;
			for (it = ret.first; it != ret.second; ++it)
			{
				Borrowing *b = bb = it->second;
				if (knjiga == b->book())
				if (!b->vracena()){
					b->vratio(bcd); bookFound = true;
					break;
				}
			}
			if (bookFound) {
				cout << "Unesite Poziciju na koju bi vratili knjigu(prvo Prostorija, Ormar pa policu)" << endl;
				int a, b, c; cin >> a; cin >> b; cin >> c; if (a < 0 || b < 0 || c < 0) throw new BadNumber();
				bb->book()->taken_position(new Position(a, b, c));
			}
		}
	};
	class Member{
		bool ismember;
		int num = 0, licna;
		Person *p;
	public:  //za pocetak.
		~Member(){ p = nullptr; }
		Member(Person *pe) :p(pe), ismember(1){ licna = pe->myID(); }
		friend ostream& operator<<(ostream &it, const Member* m){ it << m->p; return it; }
		bool isMember()const{ return ismember; }
		void delMember(){ ismember = 0; }
		void makeMember(){ ismember = 1; }
		bool good(){ return num == 0; }
		void borrow(){ num++; }
		void returrn(){ num--; }
		bool more(){ return num >= N; }
		int possesion()const{ return num; }
		Person* person(){ return p; }
	};
	unordered_map<int, Member*> members;
	BookCollection *books;
	FilesInLibrary *files;
	Member* check_member(int memID)
	{
		unordered_map<int, Member *>::iterator it = members.find(memID);
		if (it == members.end()) return nullptr;
		return it->second;
	}
	Book* check_book(int bookID)
	{
		if (!books) return nullptr;
		return books->lookAtBook(bookID);
	}												//FileStorage *files;
public:
	Result* search_Author(string autor)
	{
		Result *r = books->search_Author(autor);
		return r;
	}
	void ispis_Dat(string cla="Clanovi.txt",string knji="Knjige.txt",string poz="Pozajmice.txt")
	{
		ofstream clanovi, knjige, pozajmice;
		clanovi.open(cla);
		allMembers(clanovi);
		clanovi.close();
		knjige.open(knji);
		allBooks(knjige);
		knjige.close();
		pozajmice.open(poz);
		allBorrowed(pozajmice);
		pozajmice.close();
	}
	void undo_Borrowing(int memberID, int bookID, Date *from, Date *to)
	{
		Book *vrati = check_book(bookID);
		if (vrati == nullptr) return; //PRIJA\vi GRESKU
		files->undo_Borrowing(memberID, vrati, to, from);
	}
	int zamena(){
		if (books) return books->zamena();
		else return 0;
	}
	int pozajmljene(){
		if (books) return books->pozajmljene();
		else return 0;
	}
	int numOfBooks(){
		if (books) return books->numOfBooks();
		else return 0;
	}
	~Library(){
		delete files; files = nullptr;
		delete books; books = nullptr;
		for (unordered_map<int, Member*>::iterator it = members.begin(); it != members.end(); ++it)
			delete it->second;
	}
	Result* search_Name_Book(string naziv, string nazizd = "", int godina = 0){
		Result *r = books->search_Name_Book(naziv, nazizd, godina);
		return r;
	}
	Result* search_Date(Date *d){
		Result *r = books->search_Date(d);
		return r;
	}
	Result * search_publication(int year)
	{
		Result *r = books->search_publication(year);
		return r;
	}

	Result* search_Condition(BookCondition c){
		Result *r = books->search_Condition(c);
		return r;
	}
	static void setN(int n){ N = n; }
	void istekaoRok(Date *datum){
		if (files) files->istekaoRok(datum);
	}
	void izmedju(Date *begin, Date *end)
	{
		if (files) files->izmedju(begin, end);
	}
	Library() :books(nullptr), files(nullptr){
		members.clear();
	}
bool addMember(Person *p)
	{
		int memberID = p->myID();
		unordered_map<int, Member*>::iterator it = members.find(memberID);
		if (it == members.end())
		{
			members.emplace(memberID, new Member(p));
			cout << "Uspesno Uclanjenje" << endl;
			p->setMemID(memberID);
			return true;
		}
		else
		if (!it->second->isMember())
		{
			it->second->makeMember();
			cout << "Osoba je vec clan biblioteke" << endl;
			p->setMemID(memberID);
			return true;
		}
	return false;
	}
	bool addBook(Book *knjiga)
	{
		bool vrati = false;;
		if (!books) books = new BookCollection();//(this);
		if (books->lookAtBook(knjiga->getBookID()) == nullptr)
		{
			knjiga->setCondition(NOVA); knjiga->addBookID(++bookid); books->addBook(knjiga); vrati = true;
		}
		else cout << "Knjiga se vec nalazi u biblioteci" << endl;
		return vrati;
	}
	bool removeMember(int memID)
	{
		unordered_map<int, Member*>::iterator it = members.find(memID);
		if (it == members.end()) { cout << "Clan Nije Ni Uclanjen" << endl; return false; }
		if (it != members.end() && it->second->isMember() && it->second->good()){
			it->second->delMember();
			cout << "Uspesno Isclanjenje" << endl;
			return true;
		}
		return false;
	}
	bool removeBook(int bookID)
	{
		bool vrati = false;
		if (books){
			if (books->lookAtBook(bookID) != nullptr)
			{
				books->removeBook(bookID);
				vrati = true;
			}
		}
		return vrati;
	}
	void removeBook_Completely(int bookID)
	{
		if (books) books->removeBook_Completely(bookID);
	}
	Person *readMember(int memID) ///////////////////CH
	{
		unordered_map<int, Member *>::iterator it = members.find(memID);
		if (it == members.end()) return nullptr;
		return it->second->person();
	}
	Book *readBook(int bookID)
	{
		return check_book(bookID);
	}
	void member_borrowings(int memID)
	{
		if (files) files->borrowings(memID);
	}
	void allBorrowed(ostream &itok = cout)const
	{
		if (files) files->all(itok);
	}
	void allMembers(ostream &itok = cout)
	{
		unordered_map<int, Member*>::iterator it = members.begin();
		itok << "ALL MEMBERS:" << endl;
		for (; it != members.end(); ++it)
		{
			itok << it->second; if (!it->second->isMember()) itok << "(Isclanjen)"; itok << endl;
		}
		itok << endl;
	}
	void allBooks(ostream &itok = cout)
	{
		if (books) books->printAllBooks(itok);
	}
	//void help_Borrow(int memID, string s, Date *beg, Date *end)
	//{
	//	Book* knjiga=books->askForBook(s);
	//	borrow(memID, knjiga->getBookID(), beg, end);
	//}
	bool borrow(int memID, int bookID, Date *beg, Date *end)
	{
		Member *mem = check_member(memID);
		if (!mem || !mem->isMember()) { cout << "Nema Uclanjenog Sa Takvim ID-om" << endl; return false; }
		if (mem->more()){ cout << " Clan vec poseduje " << mem->possesion() << " Sto je i maksimum." << endl; return false; }
		Book *knjiga = check_book(bookID);
		if (knjiga == nullptr) { cout << "Nepostojeca Knjiga" << endl; return false; }
		if (knjiga->getCondition() == UKLONJENA) { cout << " Knjiga je uklonjena iz upotrebe. " << endl; return false; }
		if (knjiga->location()->isBookInLibrary())
		{
			if (!files) files = new FilesInLibrary(this);
			files->borrow_Book(memID, knjiga, beg, end);
			mem->borrow();
			cout << "Uspesno Pozajmljena Knjiga" << endl;
			return true;
		}
		else {
			cout << "Knjiga Je Trenutno Pozajmljena" << endl; return false;
		}
	}
	void returnBook(int memID, int bookID, BookCondition bcd)
	{
		Member *mem = check_member(memID);
		if (!mem->isMember()) { cout << "Nema Uclanjenog Sa Takvim ID-om"; return; }
		Book *knjiga = check_book(bookID);
		if (!knjiga) { cout << "Nepostojeca Knjiga"; return; }
		if (files) {
			files->return_Book(memID, knjiga, bcd);
			mem->returrn();
		}
	}
	//void allPublishers()const{Books::sviIzdavaci();}
};

int Library::N = 10;

class Operation{
	static ofstream *upis;
	const string currentDateTime() {
		time_t     now = time(0);
		struct tm  tstruct;
		char       buf[80];
		tstruct = *localtime(&now);

		strftime(buf, sizeof(buf), "%Y-%m-%d, %X", &tstruct);

		return buf;
	}
	static bool admin;
protected:
	static Library *biblioteka;
public:
	virtual ~Operation(){}
	static void setLibrary(Library *l){ biblioteka = l; }
	static void setLogging(bool adm)
	{
		admin = adm;
		upis = new ofstream;
		upis->open("Dnevnik_Rada.txt");
	}
	static void endLogging()
	{
		upis->close();
	}
	virtual string getString() const = 0;
	virtual void log()
	{
		*upis << currentDateTime() << " " << getString();
		if (admin) *upis << " ::ADMIN::";		cout << endl;
	}
	virtual void execute() = 0;
	virtual void undo(){}
};

Library* Operation::biblioteka = nullptr;
bool Operation::admin = false;
ofstream* Operation::upis = nullptr;

class UndoStack :private stack<Operation*>
{
	Operation* pop(){
		if (stack<Operation*>::empty()) return nullptr;
		Operation *cur = stack<Operation*>::top();
		stack<Operation*>::pop();
		return cur;
	}
public:
	void undo()
	{
		Operation *cur = this->pop();
		if (cur != nullptr) cur->undo();
	}
	void push(Operation *ins)
	{
		stack<Operation*>::push(ins);
	}
	~UndoStack(){
		while (!stack<Operation*>::empty())
		{
			Operation * del = pop();
			delete del;
		}
	}
};

class ModifyingOperation : public Operation{

public:
	void log() override
	{
		Operation::log();
	}
};

class OperationEngine
{
	//ifstream *dnevnik;
	UndoStack *history;
	bool rezim;
	Operation *op;
	bool admin;
public:
	~OperationEngine(){ delete history; Operation::endLogging(); delete op; }
	void submitOperation(Operation *o)
	{
		op = o;
	}
	void run()
	{
		if (op)
		{
			op->execute();
			if (rezim) op->log();
			if (dynamic_cast<ModifyingOperation*>(op)) {
				history->push(op);
			}
			/*try{
				throw op;
			}
			catch (ModifyingOperation *mo)
			{
				history->push(mo);
			}
			catch (...)
			{

			}*/
		}
		op = nullptr;
	}
	void enableLogging()
	{
		Operation::setLogging(admin);
	}
	OperationEngine(bool r, bool adm) :rezim(r), admin(adm)
	{
		if (rezim) enableLogging();
		//dnevnik = new ifstream();
		//dnevnik->open("Dnevnik_Rada.txt");
		history = new UndoStack();
	}
	void undo()
	{
		history->undo();
	}
};

//////////////////////////////////////
class ViewingOperation : public Operation{
public:
	void log() override
	{
		Operation::log();
	}
};

class LibraryState : public ViewingOperation{
	string s = "Ispisivanje Stanja U Biblioteci";
public:
	string getString()const override{ return s; }
	void log() override
	{
		Operation::log();
	}
	void execute()
	{
		 
		cout << "Ukupno Knjiga u Vlasnistvu Biblioteke:" << biblioteka->numOfBooks() << endl;
		cout << "Trenutno Pozajmljene" << biblioteka->pozajmljene() << endl;
		cout << "Istekao rok";  cout<<biblioteka->search_Date(new Date(1, 1, 1)); cout << endl;////random date.
		cout << "Potrebno Zameniti" << biblioteka->zamena() << endl;
	}
};

class PrintMemberBorrowings : public ViewingOperation{
	string s = "Ispisivanje Pozajmljivanih Knjiga Clana";
	int memberID;
public:
	PrintMemberBorrowings(int id) :memberID(id){}
	string getString()const override{ return s; }
	void log() override
	{
		Operation::log();
	}
	void execute()
	{
		 
		biblioteka->member_borrowings(memberID);
	}
};

class PrintPosition : public ViewingOperation{
	string s = "Ispisivanje Pozicije Odredjene Knjige";
	Book *knjiga;
public:
	PrintPosition(Book *b) :knjiga(b){}
	~PrintPosition(){ knjiga = nullptr; }
	string getString()const override{ return s; }
	void log() override
	{
		Operation::log();
	}
	void execute()
	{
		 
		knjiga->position();
	}
};

class PrintState : public ViewingOperation{
	string s = "Ispisivanje Stanja Odredjene Knjige";
	Book *knjiga;
public:
	PrintState(Book *b) :knjiga(b){}
	~PrintState(){ knjiga = nullptr; }
	string getString()const override{ return s; }
	void log() override
	{
		Operation::log();
	}
	void execute()
	{
		 
		cout << niz[knjiga->getCondition()];
	}
};

class PrintBorrowings : public ViewingOperation{
	string s = "Ispisivanje Pozajmica";
public:
	string getString()const override{ return s; }
	void log() override
	{
		Operation::log();
	}
	void execute()
	{
		 
		biblioteka->allBorrowed();
	}
};

class PrintMembers : public ViewingOperation{
	string s = "Ispisivanje Clanova";
public:
	string getString()const override{ return s; }
	void log() override
	{
		Operation::log();
	}
	void execute()
	{
		 
		biblioteka->allMembers();
	}
};

class PrintBooks : public ViewingOperation{
	string s = "Ispisivanje Knjiga";
public:
	string getString()const override{ return s; }
	void log() override
	{
		Operation::log();
	}
	void execute()
	{
		biblioteka->allBooks();
	}
};

class PrintResults :public ViewingOperation{
	string s = "Ispisivanje Resulta";
	Result *rez;
public:
	PrintResults(Result *r):rez(r){}
	~PrintResults(){ rez = nullptr; }
	string getString()const override{ return s; }
	void log() override
	{
		Operation::log();
	}
	void execute()
	{
		 
		cout << rez;//ili *rez;
	}
};

//////////////////////////////////////////////

class AddingBook : public ModifyingOperation{
	string s = "Ubacivanje Knjige";
	Book *knjiga;
	bool change;
public:
	AddingBook(Book* b) :knjiga(b){}
	~AddingBook(){ knjiga = nullptr; }
	string getString()const override{ return s; }
	void log() override
	{
		ModifyingOperation::log();
	}
	void execute()
	{
		 
		change = biblioteka->addBook(knjiga);
		if (change){
			knjiga->setCondition(NOVA);
			cout << "Unesite Poziciju Na koju hocete da postavite knjigu(Prvo br.Prostorija, Ormara pa police)" << endl;
			int pr, ore, po; cin >> pr; cin >> ore; cin >> po;
			if (pr <= 0 || ore <= 0 || po <= 0)throw new BadNumber();
			Position *pos = new Position(pr, ore, po);
			knjiga->taken_position(pos);
		}
	}
	void undo()override
	{
		if (change) biblioteka->removeBook_Completely(knjiga->getBookID());
	}
};

class ResolvingBook : public ModifyingOperation{
	string s = "Ubacivanje Knjige";
	Book *knjiga;
	int bookCondition;
	bool change;
public:
	~ResolvingBook(){ knjiga = nullptr; }
	ResolvingBook(Book* b) :knjiga(b), change(false){}
	string getString()const override{ return s; }
	void log() override
	{
		ModifyingOperation::log();
	}
	void execute()
	{
		 
		if (!knjiga) return;
		bookCondition = knjiga->getCondition();
		change = biblioteka->removeBook(knjiga->getBookID());
	}
	void undo()override
	{
		if (change) knjiga->setCondition((BookCondition)bookCondition);
	}
};

class UnsubscribeMember :public ModifyingOperation{
	string s = "Isclanjivanje Osobe";
	Person *person;
	bool change;
public:
	~UnsubscribeMember(){ person = nullptr; }
	UnsubscribeMember(Person *p) :person(p){}
	string getString()const override{ return s; }
	void log() override
	{
		ModifyingOperation::log();
	}
	void execute()
	{
		change = biblioteka->removeMember(person->myMemID());
	}
	void undo()override
	{
		if (change) biblioteka->addMember(person);
	}
};

class SubscribeMember :public ModifyingOperation{
	string s = "Uclanjenje Osobe";
	Person *person;
	bool change;
public:
	~SubscribeMember(){ person = nullptr; }
	SubscribeMember(Person *p) :person(p){}
	string getString()const override{ return s; }
	void log() override
	{
		ModifyingOperation::log();
	}
	void execute()
	{
		change = biblioteka->addMember(person);
	}
	void undo()override
	{
		if (change) biblioteka->removeMember(person->myMemID());
	}
};

class ReturningBook : public ModifyingOperation{
	string s = "Vracanje Knjige";
	int bookID;
	int memberID;
	BookCondition stanje;
public:
	ReturningBook(int m, int k, BookCondition st) :bookID(k), memberID(m), stanje(st){}
	string getString()const override{ return s; }
	void log() override
	{
		ModifyingOperation::log();
	}
	void execute()
	{
		biblioteka->returnBook(memberID, bookID, stanje);
	}

};

class BorrowingBook : public ModifyingOperation{
	string s = "Pozajmljivanje Knjige";
	int bookID;
	int memberID;
	Date *from, *to;
	bool change;
public:
	~BorrowingBook(){ delete from; delete to; }
	BorrowingBook(int menu, int knjiga, Date *d1, Date *d2) :memberID(menu), bookID(knjiga), from(d1), to(d2){}
	string getString()const override{ return s; }
	void log() override
	{
		ModifyingOperation::log();
	}
	void execute()
	{
		 
		change = biblioteka->borrow(memberID, bookID, from, to);
	}
	void undo()override
	{
		if (change) biblioteka->undo_Borrowing(memberID, bookID, from, to);
	}
};

class ChangingState : public ModifyingOperation{
	string s = "Menjanje Stanja Knjige";
	Book *knjiga;
	BookCondition staro;
public:
	~ChangingState(){ knjiga = nullptr; }
	ChangingState(Book *knj) :knjiga(knj){}
	string getString()const override{ return s; }
	void log() override
	{
		ModifyingOperation::log();
	}
	void execute()
	{
		 
		if (knjiga == nullptr) return;
		cout << "Unesite: " << endl;
		for (int i = 0; i < 6; i++)
			cout << i << "Za " << niz[i];
		int novo; cin >> novo;
		if (novo < 0 || novo >= 6) throw new BadNumber();
		staro = (BookCondition)knjiga->getCondition();
		knjiga->setCondition((BookCondition)novo);
	}
	void undo()override
	{
		if (knjiga != nullptr) knjiga->setCondition(staro);
	}
};

class DataWrite :public ModifyingOperation{
	string s = "Ispis Podataka U Datoteke";
	string knjige, clanovi, pozajmice;
public:
	DataWrite(string knj = "Knjige.txt", string cl = "Clanovi.txt", string poz = "Pozajmice.txt") :knjige(knj), clanovi(cl), pozajmice(poz){}
	string getString()const override{ return s; }
	void log() override
	{
		ModifyingOperation::log();
	}
	void execute()
	{
		 
		biblioteka->ispis_Dat(clanovi, knjige, pozajmice);
	}
};

///////////////////////////***********************************
class SearchingOperation : public Operation
{
public:
	void log() override
	{
		Operation::log();
	}
};


class SearchBooks_Name_Date_Pub : public SearchingOperation{
	string s = "Pretraga Knjiga Po Imenu, Izdavacu i Godini Izdanja";
public:
	string getString()const override{ return s; }
	void log()
	{
		SearchingOperation::log();
	}
	void execute()
	{
		 
		cout << "Unesite Naziv Knjige, Kao Dzoker Znak Za Zamenu Odredjenog Slova Unesite $ " << endl;
		string name, publisher; cin >> name; cout << endl;
		cout << "Unesite Izdavaca i godinu Izdanja" << endl;
		int year; cin >> publisher; cin >> year;
		cout<<biblioteka->search_Name_Book(name, publisher, year);
	}
};

class SearchBooks_Name : public SearchingOperation{
	string s = "Pretraga Knjiga Po Imenu";
public:
	string getString()const override{ return s; }
	void log()
	{
		SearchingOperation::log();
	}
	void execute()
	{
		 
		cout << "Unesite Naziv Knjige, Kao Dzoker Znak Za Zamenu Odredjenog Slova Unesite $ " << endl;
		string name; cin >> name; cout << endl;
		cout<<biblioteka->search_Name_Book(name);
	}
};

class SearchBooks_Date : public SearchingOperation{
	string s = "Pretraga Knjiga Po Datmu Pozajmice";
public:
	string getString()const override{ return s; }
	void log()
	{
		SearchingOperation::log();
	}
	void execute()
	{
		 
		cout << "Unesite Datum Pretrage, Dan, Mesec pa Godinu." << endl;
		int dd, mm, yy; cin >> dd; cin >> mm; cin >> yy;
		cout<<biblioteka->search_Date(new Date(yy, mm, dd));
	}
};

class SearchBooks_State : public SearchingOperation{
	string s = "Pretraga Knjiga Po Stanju Knjige";
public:
	string getString()const override{ return s; }
	void log()
	{
		SearchingOperation::log();
	}
	void execute()
	{
		 
		cout << "Unesite Neko Od Stanja" << endl;
		for (int i = 0; i < 6; i++)
			cout << i << "Za " << niz[i];
		int b; cin >> b;
		if (b < 0 || b >= 6) throw new BadNumber();
		cout<<biblioteka->search_Condition(nizz[b]);
	}
};

class SearchBooks_Author : public SearchingOperation
{
	string s = "Pretraga Knjiga Po Imenu Autora";
	string autor;
public:
	SearchBooks_Author(string a) :autor(a){}
	string getString()const override{ return s; }
	void log()
	{
		SearchingOperation::log();
	}
	void execute()
	{
		 
		cout<<biblioteka->search_Author(autor);
	}
};

class SearchPredicate
{
protected:
	static Library *lib;

public:
	static void setLib(Library *l){ lib = l; }
	virtual Result* operator()() = 0;
};

Library* SearchPredicate::lib = nullptr;

class AuthorNamePredicate :public SearchPredicate
{
	string name;
public:
	AuthorNamePredicate(string n) :name(n){}
	Result* operator()()override
	{
		Result *result;
		result = lib->search_Author(name);
		return result;
	}
};

class PublicationYearPredicate : public SearchPredicate
{
	int number;
public:
	PublicationYearPredicate(int num) :number(num){}
	Result *operator()()override
	{
		Result *result;
		result = lib->search_publication(number);
		return result;
	}
};

class LogicalOperator : public SearchPredicate
{
protected:
	SearchPredicate *sp1, *sp2;
	LogicalOperator(SearchPredicate *s1, SearchPredicate*s2) :sp1(s1), sp2(s2){}
public:
	~LogicalOperator(){ delete sp1; delete sp2; }
};

class OR :public LogicalOperator
{
public:
	OR(SearchPredicate *s1, SearchPredicate *s2) :LogicalOperator(s1, s2){}
	Result* operator()()override
	{
		Result *result = new Result(), *r1, *r2;
		r1 = (*sp1)();
		r2 = (*sp2)();
		*result += r1;
		*result += r2;
		delete r1;
		delete r2;
		return result;
	}
};

class AND :public LogicalOperator
{
public:
	AND(SearchPredicate *s1, SearchPredicate *s2) :LogicalOperator(s1, s2){}
	Result * operator()()override
	{
		Result *result = new Result(), *r1, *r2;
		r1 = (*sp1)();
		r2 = (*sp2)();
		for (int i = 0; i<r1->getNum(); i++)
			if (r2->in((*r1)[i])) *result += (*r1)[i];
		return result;
	}
};

class NOT :public LogicalOperator
{
public:
	NOT(SearchPredicate *s1, SearchPredicate *s2) :LogicalOperator(s1, s2){}
	Result *operator()() override
	{
		Result *result = new Result(), *r1, *r2;
		r1 = (*sp1)();
		r2 = (*sp2)();
		for (int i = 0; i<r1->getNum(); i++)
			if (!r2->in((*r1)[i])) *result += (*r1)[i];
		return result;
	}
};

//class Search
//{
//	SearchPredicate **pred;
//	int num, kap;
//public:
//	Search(int k) :kap(k), num(0)
//	{
//		pred = new	SearchPredicate*[kap];
//	}
//	void find()
//	{
//
//		for (int i = 0; i < num; i++)
//			(*pred[i])()
//	}
//};

class SearchBooks_Combined : public SearchingOperation
{
	string s = "Kombinovana Pretraga Knjiga";
	SearchPredicate *combine;
public:
	~SearchBooks_Combined(){ delete combine; }
	SearchBooks_Combined(SearchPredicate *sp) :combine(sp){}
	string getString()const override{ return s; }
	void log()
	{
		SearchingOperation::log();
	}
	void execute()
	{
		Result *rez;
		rez=(*combine)();
		cout << rez;
	}
};

class SearchMembers_Range :public SearchingOperation{
	string s = "Pretraga Clanova Na Osnovu Opsega Datuma Pozajmljivanja";
public:
	string getString()const override{ return s; }
	void log()
	{
		SearchingOperation::log();
	}
	void execute()
	{
		 
		cout << "Unesite Pocetni Datum Pretrage, Dan, Mesec pa Godinu." << endl;
		int dd, mm, yy; cin >> dd; cin >> mm; cin >> yy;
		cout << "Unesite krajnji Datum Pretrage, Dan, Mesec pa Godinu." << endl;
		int dd2, mm2, yy2; cin >> dd2; cin >> mm2; cin >> yy2;
		biblioteka->izmedju(new Date(yy, mm, dd), new Date(yy2, mm2, dd2));
	}
};

class SearchMembers_Late :public SearchingOperation{
	string s = "Pretraga Clanova Na Osnovu Zakasnjenja Zadatog Datuma";
public:
	string getString()const override{ return s; }
	void log()
	{
		SearchingOperation::log();
	}
	void execute()
	{
		 
		cout << "Unesite Datum Pretrage, Dan, Mesec pa Godinu." << endl;
		int dd, mm, yy; cin >> dd; cin >> mm; cin >> yy;
		biblioteka->istekaoRok(new Date(yy, mm, dd));
	}
};

//*************************************************
class CompositeOperation : public Operation
{
	Operation **ops;
	int kap, num;
	string s = "";
public:
	~CompositeOperation(){ for (int i = 0; i < num; i++) delete ops[i]; }
	CompositeOperation(int n) :kap(n), num(0)
	{
		ops = new Operation*[n];
	}
	string getString()const override{ return s; }

	void log()
	{
		for (int i = 0; i<num; i++)
			ops[i]->log();
	}
	void addOperation(Operation *op)
	{
		ops[num++] = op;
	}
	void execute()
	{
		for (int i = 0; i<num; i++)
			ops[i]->execute();
	}
	void undo()override
	{
		for (int i = num - 1; i >= 0; i--)
			ops[i]->undo();
	}
};
class Main{
	OperationEngine *oe;
	int num, brKnjiga;
	Library *biblioteka;
	Person **ljudi;
	Book **knjige;
	void ispisLjudi(Person **ljudi, int broj, Library *biblioteka, bool mode)
	{
		if (mode)
			cout << "Unesite Broj ljudi koje bi uclanili sada: ";
		else
			cout << "Unesite Broj Ljudi Koje Bi Isclanili sada: ";
		int k;
		cin >> k;
		if (k <= 0) throw new BadNumber();
		for (int i = 0; i < broj; i++)
		{
			cout << "Broj: " << i << " Za: " << ljudi[i] << endl;
		}
		for (int i = 0; i < k; i++)
		{
			int menu; cin >> menu;
			if (i<k - 1) cout << ", ";
			if (menu < 0 || menu >= broj) throw new BadNumber();
			if (mode)
				oe->submitOperation(new SubscribeMember(ljudi[menu]));
			else 
				oe->submitOperation(new UnsubscribeMember(ljudi[menu]));
			

			oe->run();//dovde stigao.
		}
	}
	void ispisKnjiga(Book **knjige, int broj)
	{
		for (int i = 0; i < broj; i++)
		{
			cout << "Broj: " << i << " Za "; knjige[i]->easyPrint();
		}
		cout << "Unesite Broj Koju Knjigu Birate" << endl;
	}

	void ubacivanjeKnjiga(Library *biblioteka, int broj, Book **knjige, bool mode)
	{
		if (mode)
			cout << "Unesite Broj Knjiga Koje Zelite Da Ubacite ";
		else 
			cout << "Unesite Broj Knjiga Koje Zelite Da Izbacite ";
		int k;
		cin >> k;
		if (k <= 0) throw new BadNumber();
		ispisKnjiga(knjige, broj);
		for (int i = 0; i < k; i++)
		{
			int izbor; cin >> izbor;
			if (izbor < 0 || izbor >= num) throw new BadNumber();

			if (mode)
				oe->submitOperation(new AddingBook(knjige[izbor]));
			else
			{
				Book *knj = biblioteka->readBook(knjige[izbor]->getBookID());
				oe->submitOperation(new ResolvingBook(knj));
			}
			oe->run();
		}
	}
	void pozicija(Book **knjige, int broj)
	{
		ispisKnjiga(knjige, broj);
		int a;
		cin >> a;
		if (a < 0 || a >= broj) throw new BadNumber();
			knjige[a]->position();
	}
	SearchPredicate* makePredicate(SearchPredicate *sp)
	{
		int menu;
		SearchPredicate *sp1,*sp2;
		cout << "Unesite 1. Pretragu Na Osnovu Imena Autora, 2. Pretragu Na osnovu Datuma Publikacije" << endl;
		cin >> menu;
			switch (menu)
			{
			case 1:
			{
					  cout << "Unesite Ime Autora $ predstavlja Dzoker Znak" << endl;
					  string autor; cin >> autor;
					  sp1 = new AuthorNamePredicate(autor);
					  break;
			}
			case 2:
			{
					  cout << "Unesite Godinu Publikacije " << endl;
					  int year; cin >> year;
					  sp1 = new PublicationYearPredicate(year);
					  break;
			}

			}
			cout << "Sada za drugu Pretragu:" << endl;
			cout << "Unesite 1. Pretragu Na Osnovu Imena Autora, 2. Pretragu Na osnovu Datuma Publikacije" << endl;
			cin >> menu;
			switch (menu)
			{
			case 1:
			{
					  cout << "Unesite Ime Autora $ predstavlja Dzoker Znak" << endl;
					  string autor; cin >> autor;
					  sp2 = new AuthorNamePredicate(autor);
					  break;
			}
			case 2:
			{
					  cout << "Unesite Godinu Publikacije " << endl;
					  int year; cin >> year;
					  sp2 = new PublicationYearPredicate(year);
					  break;
			}

			}
		cout << "Unesite 1 za AND / 2 za OR / 3 za NOT" << endl;
		cin >> menu;
		switch (menu)
		{
		case 1:
			sp = new AND(sp1, sp2);
			break;
		case 2:
			sp = new OR(sp1, sp2);
			break;

		case 3:
			sp = new NOT(sp1, sp2);
			break;
		}
		return sp;
	}
	void pretrazivanje_knjiga(Library *biblioteka)
	{
		cout << "Unesite Opciju :" << endl;
		cout << "1.Pretraga prema nazivu knjige" << endl;
		cout << "2.Pretraga prema isteku datuma" << endl;
		cout << "3.Pretraga Prema Stanju knjige" << endl;
		cout << "4. Za Kombinovanu Pretragu" << endl;
		int menu; cin >> menu;
		switch (menu)
		{
		case 1:
		{
				  cout << "1.Pretraga Samo po nazivu" << endl;
				  cout << "2.Po nazivu,izdavacu i godini izdanja " << endl;
				  int op; cin >> op;
				  if (op == 2)
					  oe->submitOperation(new SearchBooks_Name_Date_Pub());
				  else  oe->submitOperation(new SearchBooks_Name());
				  break;
		}
		case 2:
		{
				  oe->submitOperation(new SearchBooks_Date());
				  break;
		}
		case 3:
		{
				  
				  oe->submitOperation(new SearchBooks_State());
				  break;
		}
		case 4:
		{
				  SearchPredicate *sp = nullptr;
				  sp=makePredicate(sp);
				  oe->submitOperation(new SearchBooks_Combined(sp));
				  break;
		}
		}
		oe->run();
	}
	void pretrazivanje_clanova(Library *biblioteka)
	{
		cout << "Unesite Opciju:" << endl;
		cout << "1.Zadavanje datuma, provera da li su do tog dana vratili svi koji su trebali" << endl;
		cout << "2.Zadavanje opsega, provera ko je sve tada pozajmljivao koje knjige." << endl;
		int menu; cin >> menu;
		if (menu == 1)
			oe->submitOperation(new SearchMembers_Late());
		else 
			oe->submitOperation(new SearchMembers_Range());
		oe->run();
	}
	void readFromDat()
	{
		ifstream *datKnjige, *datLjudi;
		datKnjige = new ifstream();
		datLjudi = new ifstream();
		cout << "Unesite Ime datoteke iz koje se citaju Knjige" << endl;
		bool check = 1;
		string imeKnjiga = "knjige.txt", imeLjudi = "ljudi.txt";
		while (check)
		{
			try{
				//cin>>imeKnjiga; cin>>imeLjudi;
				datKnjige->open(imeKnjiga);
				datLjudi->open(imeLjudi);
				if (datKnjige == nullptr || datLjudi == nullptr) throw new NoFile();
				check = 0;
			}
			catch (NoFile *nf)
			{
				cout << nf;
			}
		}
		*datLjudi >> num;
		*datKnjige >> brKnjiga;
		if (num <= 0 || brKnjiga <= 0) throw new BadNumber();
		string s1, s2;
		ljudi = new Person*[num];
		for (int i = 0; i < num; i++)
		{
			*datLjudi >> s1; *datLjudi >> s2;
			ljudi[i] = new Person(s1, s2);
		}
		knjige = new Book*[brKnjiga];
		for (int i = 0; i < brKnjiga; i++)
		{
			int brar, god;
			*datKnjige >> s1; //naziv knjige
			*datKnjige >> god; //godina izdanja
			*datKnjige >> s2; //naziv izdavaca 

			*datKnjige >> brar;
			if (brar <= 0) throw new BadNumber();
			string *imena = new string[brar]; //broj autora
			string *prezimena = new string[brar];
			for (int j = 0; j < brar; j++){
				*datKnjige >> imena[j]; //ime autora
				*datKnjige >> prezimena[j];
			}
			int podatak; //podatak o izdanju
			*datKnjige >> podatak;
			string isbn, zanr, jezik;
			bool tip;
			*datKnjige >> tip;
			*datKnjige >> isbn;
			*datKnjige >> zanr;
			*datKnjige >> jezik;

			//Book(BookCondition bc, BorrowingPosition *pp, string naz, int godinaizd, Autori *aut, string nazivIzd, int podatakOIZD, string isbnn, bool tip, string zanrr, string jezikk) :
			knjige[i] = new Book(NIJEBILA, nullptr, s1, god, new Autori(brar, imena, prezimena), s2, podatak, isbn, tip, zanr, jezik);
		}
		datLjudi->close();
		datKnjige->close();
		datLjudi = datKnjige = nullptr;
	}
	void readFromKeyboard()
	{
		cout << "Prvo unesite broj ljudi koje kreirate i sve potrebne podatke za njih.";
		cin >> num;
		if (num <= 0) throw new BadNumber();
		string s1, s2;
		ljudi = new Person*[num];
		for (int i = 0; i < num; i++)
		{
			cout << "Unesite Ime i Prezime Osobe: ";
			cin >> s1; cin >> s2;
			ljudi[i] = new Person(s1, s2);
			cout << endl;
		}
		cout << "Sad unesite broj knjiga koje kreirate i sve potrebne podatke za njih.";
		cin >> brKnjiga;
		if (brKnjiga <= 0) throw new BadNumber();
		knjige = new Book*[brKnjiga];
		for (int i = 0; i < brKnjiga; i++)
		{

			cout << "Unesite sada Naziv knjige, Godinu Izdavanja, Naziv Izdavaca i Broj Autora respektivno" << endl;
			string s1, s2; int brar, god;

			cin >> s1;
			cin >> god;
			cin >> s2;

			cin >> brar;
			if (brar <= 0) throw new BadNumber();
			string *imena = new string[brar]; //broj autora
			string *prezimena = new string[brar];
			cout << "Unesite Ime Pa Prezime Autora" << endl;
			for (int j = 0; j < brar; j++){
				cin >> imena[j]; //ime autora
				cin >> prezimena[j]; //prezime
			}
			cout << "Unesite Podatak O Izdanju(int)" << endl;
			int podatak;
			cin >> podatak;
			string isbn, zanr, jezik;
			bool tip;
			cout << "Unesite tip ISBN-a, 1 za 13-ocifreni, 0 za 10-ocireni" << endl;
			cin >> tip;
			if (tip != 0 && tip != 1) throw new BadNumber();
			cout << "Unsite ISBN" << endl;
			cin >> isbn;
			if (tip && isbn.length() != 13 || !tip && isbn.length() != 10) 	throw new BadNumber();
			cout << "Unesite zanr" << endl;
			cin >> zanr;
			cout << "Unesite jezik" << endl;
			cin >> jezik;

			//Book(BookCondition bc, BorrowingPosition *pp, string naz, int godinaizd, Autori *aut, string nazivIzd, int podatakOIZD, string isbnn, bool tip, string zanrr, string jezikk) :
			knjige[i] = new Book(NIJEBILA, nullptr, s1, god, new Autori(brar, imena, prezimena), s2, podatak, isbn, tip, zanr, jezik);
		}
	}
	int selection()
	{

		int menu = 0;
		cout << "Unesite Neku Od Sledecih Opcija:" << endl;
		cout << "1.Uclanjivanje Ljudi U Biblioteku" << endl;//
		cout << "2.Ubacivanje Knjiga U Biblioteku" << endl;//
		cout << "3.Ispis Svih Knjiga u Biblioteci" << endl;
		cout << "4.Ispis Svih Clanova Biblioteke" << endl;
		cout << "5.Pozicija Odredjene Knjige" << endl;
		cout << "6.Stanje Odredjene Knjige" << endl;
		cout << "7.Promena Stanja Odredjene Knjige" << endl;
		cout << "9.Pretrage Knjiga" << endl;
		cout << "10.Pretrage Clanova" << endl;
		cout << "11.Pozajmljivanje Odredjene Knjige" << endl;
		cout << "12.Isclanjivanje Ljudi Iz Biblioteke" << endl;//
		cout << "13.Izbacivanje Knjige Iz biblioteke" << endl;//
		cout << "14.Ispis Svih Pozajmljivanih Knjiga" << endl;
		cout << "15.Ispis svih knjiga koje je pozajmljivao odredjeni clan" << endl;
		cout << "16.Vracanje Knjige" << endl;
		cout << "17.Stanje u biblioteci" << endl;
		cout << "18. Za Undo-ovanje Prethodne Operacije" << endl;
		cout << "19. Ispis Podataka Biblioteke U Datoteke" << endl;
		cout << "0.Izlaz Iz Programa" << endl;
		cin >> menu; int a;
		if (menu <= 0 || menu >= 20) throw new BadNumber(); // || menu >= ?;
		switch (menu)
		{
		case 1:{ispisLjudi(ljudi, num, biblioteka, 1); break; }
		case 12:{ispisLjudi(ljudi, num, biblioteka, 0); break; }
		case 2:{ubacivanjeKnjiga(biblioteka, brKnjiga, knjige, 1); break; }/////////////
		case 13:{ubacivanjeKnjiga(biblioteka, brKnjiga, knjige, 0); break; }/////////////
		case 3:{oe->submitOperation(new PrintBooks()); oe->run(); break; }
		case 4:{ oe->submitOperation(new PrintMembers()); oe->run(); break; }
		case 14:{oe->submitOperation(new PrintBorrowings()); oe->run();; break; }
		case 5:{pozicija(knjige, brKnjiga); break; }
		case 6:{ispisKnjiga(knjige, brKnjiga); cin >> a;
			oe->submitOperation(new PrintState(knjige[a]));
			oe->run();
			break;
		}
		case 7:
		{
				  ispisKnjiga(knjige, brKnjiga); cin >> a;
				  oe->submitOperation(new ChangingState(knjige[a]));
				  oe->run();
				  break;
		}
		case 9: {pretrazivanje_knjiga(biblioteka); break; }
		case 10:{pretrazivanje_clanova(biblioteka); break; }
		case 11:
		{
				   cout << "Izaberite Osobu Koja Pozajmljuje" << endl;
				   for (int i = 0; i < num; i++)
				   {
					   cout << "Broj: " << i << "Za: " << ljudi[i] << endl;
				   }
				   cin >> menu; //Osoba
				   if (menu < 0 || menu >= num) throw new BadNumber();
				   cout << "Izaberite sada prema IDu knjige odredjenu knjigu za pozajmljivanje" << endl;
				   biblioteka->allBooks();
				   int knjiga; cin >> knjiga;
				   cout << "Unesite Od kad do kad zelite da pozajmite knjigu (Dan, Mesec Godina)" << endl;
				   int dd, dd2, mm, mm2, yy, yy2; cin >> dd; cin >> mm; cin >> yy; cin >> dd2; cin >> mm2; cin >> yy2;
				   oe->submitOperation(new BorrowingBook(ljudi[menu]->myMemID(), knjiga, new Date(yy, mm, dd), new Date(yy2, mm2, dd2))); //////////
				   oe->run();
				   break;
		}
		case 15:
		{
				   cout << "Izaberite Osobu od interesa" << endl;
				   for (int i = 0; i < num; i++)
				   {
					   cout << "Broj: " << i << "Za: " << ljudi[i] << endl;
				   }
				   cin >> menu;
				   if (menu < 0 || menu >= num)throw new BadNumber();;
				   oe->submitOperation(new PrintMemberBorrowings(ljudi[menu]->myMemID()));
				   oe->run();
				   break;
		}
		case 18:

			{
				oe->undo();
				break;
			}
		case 19:
		{
				   oe->submitOperation(new DataWrite());
				   oe->run();
				   break;
		}
		case 16:
		{
				   cout << "Izaberite Osobu od interesa" << endl;
				   for (int i = 0; i < num; i++)
				   {
					   cout << "Broj: " << i << "Za: " << ljudi[i] << endl;
				   }
				   cin >> menu;
				   if (menu < 0 || menu >= num) throw new BadNumber();;
				   cout << "Uniesite ID knjige od interesa" << endl;
				   biblioteka->allBooks();
				   int knjiga; cin >> knjiga;
				   cout << "Unesite Trenutno stanje knjige(Izaberite Opciju)" << endl;
				   for (int i = 0; i < 6; i++)
				   {
					   cout << "i " << niz[i] << endl;
				   }
				   int st;
				   cin >> st;
				   BookCondition stanje = (BookCondition)nizz[st];
				   oe->submitOperation(new ReturningBook(ljudi[menu]->myMemID(), knjiga, stanje));
				   oe->run();
				   break;
		}
		case 17:
		{
				   oe->submitOperation(new LibraryState());
				   oe->run();
				   break;
		}
		case 0:
			return 0;
		}
			return 1;
	}
	void del()
	{
		delete biblioteka;

		for (int i = 0; i < num; i++)
			delete ljudi[i];
		delete ljudi;

		for (int i = 0; i < brKnjiga; i++)
			delete knjige[i];
		delete knjige;

		biblioteka = nullptr;
		knjige = nullptr;
		ljudi = nullptr;
		num = brKnjiga = 0;
		delete oe;
		oe = nullptr;

	}
	/*void dnevnik_Ispis()
	{
		if (!rezim) return;
		if (dnevnik != nullptr) dnevnik->close();
		dnevnik->open("Dnevnik_Rada.txt");
		if (dnevnik == nullptr) cout << "Nema Dnevnika Rada" << endl;
		string s; *dnevnik >> s;
		while (!dnevnik->eof())
		{
			for (int i = 0; i < 4 && !dnevnik->eof(); i++){
				cout << s << " ";
				*dnevnik >> s;
			}
			cout << endl;
		}
		dnevnik->close();
	}*/
public:
	Main() :biblioteka(nullptr){}
	void start(){
		cout << "Unesite 0 za Rad u normalnom rezimu(Nije dozvoljeno ispisivanje dnevnika rada) i 1 za Admin rezim rada " << endl;
		bool rezim;
		cin >> rezim;
		oe = new OperationEngine(1,rezim);
		cout << "Za kreiranje biblioteke potrebno je uneti podrazumevanu vrednost N" << endl;
		int n; cin >> n;
		if (n <= 0 || n > 100) throw new BadNumber();
		biblioteka = new Library();
		Library::setN(n);
		SearchPredicate::setLib(biblioteka);
		Operation::setLibrary(biblioteka);
		cout << "Biblioteka je Kreirana." << endl;
		cout << "Za ucitavanje Knjiga i clanova iz datoteke unesite 1\n za rucno unosenje podataka unesite 2.";
		int choise; cin >> choise;
		if (choise == 1) readFromDat();
		if (choise == 2) readFromKeyboard();
		while (selection());
	}
	~Main(){
		del();
	}
};

int main()
{
	try{
		Main *m = new Main();
		m->start();
		delete m;
	}
	catch (BadNumber *b)
	{
		cout << b;
	}
	catch (...)
	{
		cout << "***NEOCEKIVANA GRESKA***" << endl;
	}
}
