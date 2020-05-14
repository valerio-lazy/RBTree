#ifndef __MY_TOOL_
#define __MY_TOOL
#include <utility>
#include <iostream>
#include <queue>

//默认排序仿函数
template<class T>
struct less
{
    bool operator ()(const T& x, const T& y) const { return x<y; }   
};
template<class T, class Compare>
class Set;

enum Color{
    RED,BLACK
};

//红黑树
template<class T>
struct RBNode
{
    RBNode* parent_;
    RBNode* left_;
    RBNode* right_;
    T data;
    Color color;
    
    RBNode():parent_(nullptr), left_(nullptr), right_(nullptr), data(T()), color(RED){}
    RBNode(const T& key):parent_(nullptr), left_(nullptr), 
            right_(nullptr),data(key), color(RED){}
    RBNode(T&& key):parent_(nullptr), left_(nullptr), 
            right_(nullptr),data(std::move(key)), color(RED){}
    RBNode(const RBNode* node):parent_(node->parent_), left_(node->left_), 
            right_(node->right_),data(node->data), color(node->color){}
    RBNode(const RBNode& node):parent_(node.parent_), left_(node.left_), 
            right_(node.right_),data(std::move(node.data)), color(node.color){}
    RBNode(RBNode&& node):parent_(node.parent_), left_(node.left_), 
            right_(node.right_),data(std::move(node.data)), color(node.color){}
    T& operator *(){
        return data;
    }
    T& operator ->(){
        return &(operator*());
    }
    
    RBNode& operator =(RBNode&& x){
        parent_ = x.parent_;
        left_ = x.left_;
        right_ = x.right_;
        data = x.data;
        color = x.color;
        return *this;

    }
    ~RBNode(){
        /* std::cout << "Node " << data << "is deleted" << std::endl; */
        parent_ = left_ = right_ = nullptr;
    }

    void chg_color(){
        color = (color == RED)?BLACK:RED;
    }

};
template<class T, class Compare = less<T> >
class RBTree
{
public:
    typedef RBNode<T> Node;
    RBTree():m_size(0){
        root = (Node*)operator new(sizeof(struct RBNode<T>));
        root->parent_ = root->left_ = root->right_ = nullptr;
        root->color = BLACK;
    }
    RBTree(const RBTree& x);
    RBTree(RBTree&& x);
    ~RBTree(){
        if(root->left_ != root->right_){
            destroy(root->left_);
        }
        operator delete(root);
        root = nullptr;
    }

    RBTree& operator =(RBTree&& x){
        root = x.root;
        root = nullptr;
        m_size = std::move(x.m_size);
    }

    Node* search(const T& v, bool& flag, const Compare& com = Compare());   
    Node* insert(const T& v, const Compare& comp=Compare());
    Node* remove(const T& v);
    void print() const;

protected:
    Node* l_rotate(Node* node);
    Node* r_rotate(Node* node);
    Node* far_left() const;
    Node* far_right() const;
    void open(){
        if(root->left_){
            Node* last = root->left_;
            while(last->right_ && last->right_ != root){
                last = last->right_;
            }
            last->right_ = nullptr;
        }
    }
    void close(){
        Node* last = root;
        while(last->right_){
            last = last->right_;
        }
        last->right_ = root;
    }
    void destroy(Node* node);
    Node* root; //根结点(为空)
    size_t m_size;
    friend class Set<T,Compare>;

};
template<class T, class Compare>
RBTree<T,Compare>::RBTree(const RBTree& x)
{
    m_size = 0;
    root = (Node*)operator new(sizeof(struct RBNode<T>));
    root->parent_ = root->left_ = root->right_ = nullptr;
    root->color = BLACK;
    Node* tmp = x.root->left_;
    if(!tmp){
        return;
    }
    Node* end  = x.far_right();
    /* std::cout << "end: " << end->data <<  std::endl; */
    
    while(tmp != end){
        insert(tmp->data);
    /* std::cout << "OK" << std::endl; */
        while(tmp->left_){
            tmp = tmp->left_;
            insert(tmp->data);
    /* std::cout << "OK" << std::endl; */
        }

        while(!tmp->right_)
        {
            while(tmp == tmp->parent_->right_){
                tmp = tmp->parent_;
            }
            tmp = tmp->parent_;
        }
        tmp = tmp->right_;
    }
    insert(tmp->data);
}
template<class T, class Compare>
RBTree<T,Compare>::RBTree(RBTree&& x):root(x.root),m_size(std::move(x.m_size_))
{
    x.root = nullptr;
}
template<class T, class Compare>
void RBTree<T,Compare>::print() const
{
	RBNode<T> *p;
	std::queue<RBNode<T> *> Q;
    if(root->left_ == nullptr){
        std::cout << "无结点" << std::endl;
        return;
    }
	Q.push(root->left_);
	while(!Q.empty())
	{
		p = Q.front();
		Q.pop();
		std::cout<<"节点: "<<p->data<<" ";
    /* std::cout << (void*)root << " OK " << (void*)p << std::endl; */
		if(p->left_)
		{
			std::cout<<"left:"<<p->left_->data<<"->color:"<<p->left_->color<<" ";
			Q.push(p->left_);
		}
		if(p->right_)
		{
            if(p->right_ != root){
                std::cout<<"right:"<<p->right_->data<<"->color:"<<p->right_->color<<" ";
                Q.push(p->right_);
            }
		}
		std::cout<<std::endl<<std::endl;
	}
}
template<class T, class Compare>
RBNode<T>* RBTree<T,Compare>::search(const T& v, bool& flag, const Compare& comp)
{
    /*
     *  查找值为V的结点
     *  --找到，传回该结点指针,返回true
     *  --未找到，传回该结点要插入位置的父结点,返回false
     */
    Node* ret = nullptr;
    flag = false;
    if(!root->left_){
        return root;
    }
    Node* curNode = root->left_;

    while(curNode || curNode == root){
        ret = curNode;
        if(v == curNode->data){
            flag = true;
            break;
        }
        
        if(comp(v, curNode->data)){
            curNode = curNode->left_;
        }else{
            curNode = curNode->right_;
        }
    std::cout << "search" << std::endl;
    }
    return ret;
}
template<class T, class Compare>
RBNode<T>* RBTree<T, Compare>::insert(const T& v, const Compare& comp)
{
    open();
    bool flag = false;
    Node* pNode = search(v, flag);    //要插入位置的父结点
    std::cout << "insert" << v << std::endl;
    if(flag){
        close();
        return pNode;
    }

    ++m_size;
/* 插入节点，如果插入父结点为root，设为根节点
 * 自平衡处理
 */
    Node* newNode = new Node(v);
    newNode->parent_ = pNode;
    if(pNode == root){
        pNode->left_ = pNode->right_ = newNode;
        newNode->color = BLACK;
    }else if(comp(v, pNode->data)){
        pNode->left_ = newNode;
    }else{
        pNode->right_ = newNode;
    }

    Node* curNode = newNode;
    //当父结点为黑色(空)时，完成调整
    while(pNode != nullptr && pNode->color != BLACK){
        //gpNode祖父结点, pNode父结点, unNode叔父结点
        Node* gpNode = pNode->parent_;
        Node* unNode = (pNode == gpNode->right_)?gpNode->left_:gpNode->right_;

        //case 1:叔父结点为红色
        if(unNode != nullptr && unNode->color == RED){
            gpNode->color = RED;
            pNode->color = unNode->color = BLACK;
            curNode = gpNode;
            pNode = curNode->parent_;
            if(pNode == root){
                curNode->color = BLACK;
                break;
            }
            continue;
        }
        /* case 2: 叔父结点为黑或不存在 */
        
        if(pNode == gpNode->left_){
        //2.1: 父结点是祖父结点的左子结点
            if(curNode == pNode->right_){
            //2.1.2 当前结点是父结点的右子结点(转成2.1.1)
                l_rotate(pNode);
                curNode = pNode;
                pNode = curNode->parent_;
            }
            //2.1.1 当前结点是父结点的左子结点
            pNode->color = BLACK;
            gpNode->color = RED;
            r_rotate(gpNode);
        }
        else{
        //2.2: 父结点是祖父结点的右子结点
            if(curNode == pNode->left_){
            //2.2.2 当前结点是父结点的左子结点(转成2.2.1)
                r_rotate(pNode);
                curNode = pNode;
                pNode = curNode->parent_;
            }
            //2.1.1 当前结点是父结点的左子结点
            pNode->color = BLACK;
            gpNode->color = RED;
            l_rotate(gpNode);
        }
        curNode = gpNode;
        pNode = curNode->parent_;
    }
    close();
    std::cout << "P" << std::endl;
    return newNode;
}
template<class T, class Compare>
RBNode<T>* RBTree<T, Compare>::remove(const T& v)
{
    bool flag = true;
    Node* delNode = search(v, flag);
    if(!flag){
        //如果目标不存在，返回要插入位置的父结点
        return delNode;
    }
    --m_size;
    open();
    //如果被删结点有两个子结点
    if(delNode->left_ && delNode->right_){
        Node* rpNode = delNode->left_;
        while(rpNode->right_){
            rpNode = rpNode->right_;
        }

        delNode->data = rpNode->data;
        delNode = rpNode;
    }

    Node* N = nullptr; //被删点子结点
    Node* P = delNode->parent_;
    //此时被删结点如果有唯一子结点
    if(delNode->left_ || delNode->right_){
        //用子结点替换删除结点
        N = (delNode->left_)?delNode->left_:delNode->right_;
        N->parent_ = P;
    }
    if(P->left_ == delNode){
        P->left_= N;
    }
    if(P->right_ == delNode){
        P->right_ = N;
    }
    
    if(delNode->color == RED){
        //如果被删点为红色，直接删除
        delete delNode;
        close();
        return P;
    }
    delete delNode;
    delNode = nullptr;
    //被删除点为黑色且子结点是红色
    if(N && N->color == RED){
        N->color = BLACK;
        close();
        return P;
    }

    /********  被删点是黑色且子结点也是黑色 *********/
            //必然存在兄弟节点，且兄弟节点有子结点
    Node* S = (P->left_ == N)?P->right_:P->left_;   //兄弟节点
    Node* Sl = (S->left_) ? S->left_ : nullptr;     //S节点的左子结点
    Node* Sr = (S->right_) ? S->right_ : nullptr;     //S节点的左子结点
    //1、子结点是新根，无操作
    
    //4、如果S是红色    --> P为红色，S为黑色
    if(S->color == RED){
        P->color = RED;
        S->color = BLACK;
        if(S == P->right_){
            l_rotate(P);
            S = P->right_;
        }else{
            r_rotate(P);
            S = P->left_;
        }
    }

    //2、S是黑色。如果P为红色，Sl、Sr都为黑色:   P变黑，S变红
    if(P->color == RED && Sl->color == BLACK && Sr->color == BLACK){
            P->color = BLACK;
            S->color = BLACK;
            close();
            return P;
    }

    //3、S是黑色。至多只有一个子结点是黑色
    if(S == P->right_){
        if(Sr->color == BLACK){     //S同侧子结点为黑
            r_rotate(S);
            S->color = RED;
            Sl->color = BLACK;
            Sr = S;
            S = Sl;
        }
        l_rotate(P);
        S->color = P->color;
        P->color = BLACK;
        Sr->color = BLACK;
    }else{
        if(Sl->color == BLACK){     //S同侧子结点为黑
            l_rotate(S);
            S->color = RED;
            Sl->color = BLACK;
            Sl = S;
            S = Sr;
        }
        r_rotate(P);
        S->color = P->color;
        P->color = BLACK;
        Sl->color = BLACK;
    }
    close();
    return P;
}
template<class T, class Compare>
RBNode<T>* RBTree<T,Compare>::l_rotate(Node* node)
{
    Node* pNode = node->parent_;
    Node* sRNode =  node->right_;
    Node* gsLNode = sRNode->left_; 
    /*  左旋：
     *  1、右子结点sRNode变为node的父结点
     *  2、右子结点sRNode的左子结点gsLNode变为node的右子结点
     */

    sRNode->parent_ = pNode;
    if(pNode == root){
        //无处理
        pNode->left_ = pNode->right_ = sRNode;
    }else if(pNode->left_ == node){
        pNode->left_ = sRNode;
    }else{
        pNode->right_ = sRNode;
    }

    node->parent_ = sRNode;
    sRNode->left_ = node;

    node->right_ = gsLNode;
    if(gsLNode){
        gsLNode->parent_ = node;
    }

    return sRNode;
}
template<class T, class Compare>
RBNode<T>* RBTree<T, Compare>::r_rotate(Node* node)
{
    Node* pNode = node->parent_;
    Node* sLNode =  node->left_;
    Node* gsRNode = sLNode->right_; 
    /*  右旋：
     *  1、左子结点sLNode变为node的父结点
     *  2、左子结点sLNode的右子结点gsRNode变为node的左子结点
     */

    sLNode->parent_ = pNode;
    if(pNode == root){
        //无处理
        pNode->left_ = pNode->right_ = sLNode;
    }else if(pNode->left_ == node){
        pNode->left_ = sLNode;
    }else{
        pNode->right_ = sLNode;
    }

    node->parent_ = sLNode;
    sLNode->right_ = node;

    node->left_ = gsRNode;
    if(gsRNode){
        gsRNode->parent_ = node;
    }

    return sLNode;
}
template<class T, class Compare>
RBNode<T>* RBTree<T,Compare>::far_left() const 
{//返回最左节点
    Node* ret = root->left_;
    if(ret){
        while(ret->left_){
            ret = ret->left_;
        }
    }
    return ret;
}
template<class T, class Compare>
RBNode<T>* RBTree<T,Compare>::far_right() const
{//返回最左节点
    Node* ret = root->right_;
    if(ret){
        while(ret && ret!=root){
            ret = ret->right_;
        }
    }
    return ret;
}
template<class T, class Compare>
void RBTree<T,Compare>::destroy(Node* node)
{
    Node* pNode = node->parent_;
    if(pNode == root){
        root->left_ = root->right_ = nullptr;
        return;
    }else if(node == pNode->left_){
        pNode->left_ = nullptr;
    }else{
        pNode->right_ = nullptr;
    }

    Node* temp = node;
    Node* delNode = node;
    root->parent_ = node;
    while(delNode){
        temp->parent_ = nullptr;
        if(delNode->left_){
            temp->parent_ = delNode->left_;
            temp = temp->parent_;
        }
        if(delNode->right_){
            temp->parent_ = delNode->right_;
            temp = temp->parent_;
        }
        root->parent_ = delNode->parent_;
        delete delNode;
        --m_size;
        delNode = root->parent_;
    }
}
/*************************** Iterator *************************/
template <class T>
class RBT_Iterator
{
public:
    typedef RBNode<T> Node;
    typedef RBT_Iterator<T> self;

    RBT_Iterator(){};
    RBT_Iterator(Node* node):p(node){}
    RBT_Iterator(const self& it):p(it.p){}

    self& operator ++(){
        if(!p){
            return *this;
        }
        if(p->right_ && p->right_->parent_){
            p = p->right_;
            while(p->left_){
                p = p->left_;
            }
            return *this;
        }
        while(p == p->parent_->right_){
            p = p->parent_;
            if(!p->parent_){
                while(p->right_){
                    p = p->right_;
                    if(!p->parent_){
                        break;
                    }
                }
                return *this;
            }
        }
        p = p->parent_;
        return *this;
    }
    self operator ++(int){
        self ret(*this);
        ++(*this);
        return ret;
    }

    self& operator --(){
        if(!p){
            return *this;
        }
        if(p->left_){
            p = p->left_;
            while(p->right_){
                p = p->right_;
            }
            return *this;
        }
        while(p == p->parent_->left_){
            p = p->parent_;
            if(!p->parent_){
                /* break; */
                while(p->left_){
                    p = p->left_;
                }
                return *this;
            }
        }
        p = p->parent_;
        return *this;
    }
    self operator --(int){
        self ret(*this);
        --(*this);
        return ret;

    }

    T& operator *(){
        return p->data;
    }

    bool operator !=(const self& rhs){return p != rhs.p;}
    bool operator ==(const self& rhs){return p == rhs.p;}

private:
    Node* p;
};

/*************************** Pair **************************/
//自定义简单pair
template <class KEY, class T>
struct Pair
{
    typedef KEY first_t;    //key值
    typedef T second_t;     //value

    first_t _first;
    second_t _second;

    constexpr Pair();
    Pair(const Pair& pr) = default;
    Pair(Pair&& pr) = default;
    template<class U, class V>
        Pair(const Pair<U, V>& pr): _first(pr._first), _second(pr._second){}
    template<class U, class V>
        Pair(Pair<U, V>&& pr):_first(std::move(pr._first)), _second(std::move(pr._second)){}
    template <class U, class V>
    Pair(U&& a, V&& b):_first(a), _second(b){}
};

template<class T1, class T2>
Pair<T1, T2> _make_pair(T1&& x, T2&& y)
{
    return Pair<T1, T2>(x,y);
}
#endif

