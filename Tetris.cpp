#include<SFML/Graphics.hpp>
#include<iostream>
#include<vector>
#include<string>
#include<time.h>
#include<map>
using namespace std;
using namespace sf;

struct point{
    point(int x,int y):x(x),y(y){}
    int x;
    int y;
};
const int W=18;
const int H=20;
const int squareSize=50;
vector<vector<int>> flag(H,vector<int>(W,-1));
vector<vector<int>> vp{
    {1,3,5,7}, // I
    {2,4,5,7}, // Z
    {3,5,4,6}, // S
    {3,5,4,7}, // T
    {2,3,5,7}, // L
    {3,5,7,6}, // J
    {2,3,4,5} // O
};
class resMang{
    public:
        static resMang* getins(){
            return ins;
        }
        Texture& getTexture(const string& file){
            auto it=m.find(file);
            if(it==m.end()){
                auto& texture=m[file];
                texture.loadFromFile(file);
                return texture;
            }
            return it->second;
        }
    private:
        map<string,Texture> m;
        resMang(){};
        static resMang* ins;
};
resMang* resMang::ins=new resMang();
bool check(vector<point>& a){
    for(int i=0;i<4;i++){
        if(a[i].x<0||a[i].x>=W||a[i].y>=H||a[i].y<0){
            return false;
        }else if(flag[a[i].y][a[i].x]!=-1){
            return false;
        }
    }
    return true;
}
void rotate(vector<point>& v){
    point p=v[1];
    for(int i=0;i<4;i++){
        int x=p.y-v[i].y;
        int y=p.x-v[i].x;
        v[i].x=p.x-x;
        v[i].y=p.y+y;
    }
}
//随机化一个俄罗斯方块
void randsqu(vector<point>& a,int& colorNum){
    int r=rand()%7;
    int rd=rand()%15;
    colorNum=r%4;
    for(int i=0;i<4;i++){
        a[i].x=vp[r][i]%2+rd;
        a[i].y=vp[r][i]/2;
    }
    int r1=rand();
    auto tem=a;
    for(int i=0;i<r1%4;i++){
        rotate(a);
        if(check(a)){
            tem=a;  
        }
    }
    a=tem;
}

void print(){
    for(int i=0;i<H;i++){
        for(int j=0;j<W;j++){
            cout<<flag[i][j]<<"  ";
        }
        cout<<endl;
    }
    cout<<endl;
}
void initTexture(vector<RectangleShape>& v){
    Texture texture;
    RectangleShape rec(Vector2f(squareSize,squareSize));
    auto resm=resMang::getins();
    for(int i=0;i<4;i++){
        string file="C:\\Users\\tomst\\Desktop\\snakeGame\\pic"+to_string(i)+".png";
        rec.setTexture(&(resm->getTexture(file)));
        v.push_back(rec);
    }
}

int main(){
    srand(time(0));
    sf::RenderWindow window(sf::VideoMode(900,1000), "Tetrics Game");
    Text text;
    sf::Font font;
    if(!font.loadFromFile("C:\\Users\\tomst\\Desktop\\snakeGame\\sim.ttf")){
        return -1;
    }
    text.setFont(font);
    text.setCharacterSize(60);
    text.setFillColor(Color::Red);
    text.setString("Game Over");
    text.setPosition(sf::Vector2f(250,0));
    vector<RectangleShape> recs;
    initTexture(recs);
    bool rot=false;
    bool gameover=false;
    int md=0;
    int colorNum=0;
    vector<point> now(4,point(0,0));
    vector<point> tem=now;
    randsqu(now,colorNum);
    Clock clock;
    float timer=0,delay=0.3;
    while(window.isOpen()){
        Event e;
        timer+=clock.getElapsedTime().asSeconds();
        clock.restart();
        while(window.pollEvent(e)){
            switch(e.type){
                case Event::Closed:
                    window.close();
                    break;
                case Event::KeyPressed:
                    if(e.key.code==Keyboard::W||e.key.code==Keyboard::Up){
                        rot=true;
                    }else if(e.key.code==Keyboard::A||e.key.code==Keyboard::Left){
                        md=-1;
                    }else if(e.key.code==Keyboard::D||e.key.code==Keyboard::Right){
                        md=1;
                    }
                    break;
            }
        }
        if(!gameover){
            if(Keyboard::isKeyPressed(Keyboard::Down)||Keyboard::isKeyPressed(Keyboard::S)){
                delay=0.05;
            }
            if(md){
                tem=now;
                for(auto& it:now){
                    it.x+=md;
                }
                if(!check(now)){
                    now=tem;
                }
            }
            if(rot){
                tem=now;
                rotate(now);
                if(!check(now)){
                    now=tem;
                }
            }
            if(timer>delay){
                timer=0;
                tem=now;
                for(int i=0;i<4;i++){
                    now[i].y+=1;
                }
                if(!check(now)){
                    for(int i=0;i<4;i++){
                        //添加到底的方块到记录表
                        flag[tem[i].y][tem[i].x]=colorNum;
                    }
                    randsqu(now,colorNum);
                    if(!check(now)){
                        gameover=true;
                    }
                }
            }
            rot=false;md=0;delay=0.3;
            int L=H-1;
            for(int i=H-1;i>0;i--){
                int K=0;
                for(int j=0;j<W;j++){
                    flag[L][j]=flag[i][j];
                    if(flag[i][j]!=-1){
                        K++;
                    }
                }
                if(K<W){
                    L--;
                }
            }
        }
        window.clear(Color::White);
        //print();
        for(int i=0;i<H;i++){
            for(int j=0;j<W;j++){
                if(flag[i][j]!=-1){
                auto& rec=recs[flag[i][j]];
                rec.setPosition(Vector2f(j*squareSize,i*squareSize)); 
                window.draw(rec);
                }
            }
        }
        for(int i=0;i<4;i++){
                auto& rec=recs[colorNum];
                rec.setPosition(Vector2f(now[i].x*squareSize,now[i].y*squareSize));
                window.draw(rec);
            }
        if(gameover){
            if(text.getPosition().y<300){
                text.move(0,0.1);
            }
            window.draw(text);
        }
        window.display();
    }
    return 0;
}