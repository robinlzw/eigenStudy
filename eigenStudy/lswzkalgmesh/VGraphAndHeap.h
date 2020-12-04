#pragma once

#include <string>
#include <iostream>

// Graph abstract class. This ADT assumes that the number
// of vertices is fixed when the graph is created.
class Graph 
{
private:
    void operator =(const Graph&) {} // Protect assignment
    Graph(const Graph&) {} // Protect copy constructor

public:
    Graph() {} // Default constructor
    virtual ~Graph() {} // Base destructor
    // Initialize a graph of n vertices
    virtual void Init(int n) =0;
    // Return: the number of vertices and edges
    virtual int n() =0;
    virtual int e() =0;
    // Return v’s first neighbor
    virtual int first(int v) =0;
    // Return v’s next neighbor
    virtual int next(int v, int w) =0;
    // Set the weight for an edge
    // i, j: The vertices
    // wgt: Edge weight
    virtual void setEdge(int v1, int v2, float wght) =0;
    // Delete an edge
    // i, j: The vertices
    virtual void delEdge(int v1, int v2) =0;
    // Determine if an edge is in the graph
    // i, j: The vertices
    // Return: true if edge i,j has non-zero weight
    virtual bool isEdge(int i, int j) =0;
    // Return an edge’s weight
    // i, j: The vertices
    // Return: The weight of edge i,j, or zero
    virtual float weight(int v1, int v2) =0;
    // Get and Set the mark value for a vertex
    // v: The vertex
    // val: The value to set
    virtual int getMark(int v) =0;
    virtual void setMark(int v, int val) =0;
};

template <typename E> 
class List                          // List ADT
{ 
private:
    void operator =(const List&) {} // Protect assignment
    List(const List&) {}            // Protect copy constructor

public:
    List() {}                       // Default constructor
    virtual ~List() {}         // Base destructor
    // Clear contents from the list, to make it empty.
    virtual void clear() = 0;
    // Insert an element at the current location.
    // item: The element to be inserted
    virtual void insert(const E& item) = 0;
    // Append an element at the end of the list.
    // item: The element to be appended.
    virtual void append(const E& item) = 0;
    // Remove and return the current element.
    // Return: the element that was removed.
    virtual E remove() = 0;
    // Set the current position to the start of the list
    virtual void moveToStart() = 0;
    // Set the current position to the end of the list
    virtual void moveToEnd() = 0;
    // Move the current position one step left. No change
    // if already at beginning.
    virtual void prev() = 0;
    // Move the current position one step right. No change
    // if already at end.
    virtual void next() = 0;
    // Return: The number of elements in the list.
    virtual int length() const = 0;
    // Return: The position of the current element.
    virtual int currPos() const = 0;
    // Set current position.
    // pos: The position to make current.
    virtual void moveToPos(int pos) = 0;
    // Return: The current element.
    virtual const E& getValue() const = 0;
};

class DijkElem 
{
public:
    int   vertex, preVert;
    float distance;
    DijkElem() { vertex = -1; distance = -1; preVert = -1;}
    DijkElem(int v, float d, int pv) { vertex = v; distance = d; preVert = pv;}
};

// Singly linked list node
template <typename E> 
class Link 
{
public:
    E element;                                      // Value for this node
    Link *next;                                     // Pointer to next node in list
    // Constructors
    Link(const E& elemval, Link* nextval =NULL)
    { element = elemval; next = nextval; }
    Link(Link* nextval =NULL) { next = nextval; }
};


template <typename E> 
class LList: public List<E> 
{
private:
    Link<E>* head;                                   // Pointer to list header
    Link<E>* tail;                                   // Pointer to last element
    Link<E>* curr;                                   // Access to current element
    int cnt;                                         // Size of list
    void init()                                      // Intialization helper method
    {                                    
        curr = tail = head = new Link<E>;
        cnt = 0;
    }
    void removeall()                                 // Return link nodes to free store
    {                               
        while(head != NULL) 
        {
            curr = head;
            head = head->next;
            delete curr;
        }
    }

public:
    LList(int size=1) { init(); }                      // Constructor
    ~LList() { removeall(); }                          // Destructor

    void print() const;                                // Print list contents
    void clear() { removeall(); init(); }              // Clear list
    // Insert "it" at current position
    void insert(const E& it) 
    {
        curr->next = new Link<E>(it, curr->next);
        if (tail == curr) tail = curr->next;           // New tail
        cnt++;
    }
    void append(const E& it)                          // Append "it" to list
    {                         
        tail = tail->next = new Link<E>(it, NULL);
        cnt++;
    }
    // Remove and return current element
    E remove() 
    {
        VASSERT_MSG(curr->next != NULL, "No element");
        E it = curr->next->element;                    // Remember value
        Link<E>* ltemp = curr->next;                   // Remember link node
        if (tail == curr->next) tail = curr;           // Reset tail
        curr->next = curr->next->next;                 // Remove from list
        delete ltemp;                                  // Reclaim space
        cnt--;                                         // Decrement the count
        return it; 
    }
    void moveToStart()                                // Place curr at list start
    { curr = head; }
    void moveToEnd()                                  // Place curr at list end
    { curr = tail; }
    // Move curr one step left; no change if already at front
    void prev() 
    {
        if (curr == head) return;                     // No previous element
        Link<E>* temp = head;
        // March down list until we find the previous element
        while (temp->next!=curr) temp=temp->next;
        curr = temp;
    }
    // Move curr one step right; no change if already at end
    void next()
    { if (curr != tail) curr = curr->next; }
    int length() const { return cnt; }                 // Return length
    // Return the position of the current element
    int currPos() const 
    {
        Link<E>* temp = head;
        int i;
        for (i=0; curr != temp; i++)
            temp = temp->next;
        return i;
    }
    // Move down list to "pos" position
    void moveToPos(int pos) 
    {
        VASSERT_MSG ((pos>=0)&&(pos<=cnt), "Position out of range");
        curr = head;
        for(int i=0; i<pos; i++) curr = curr->next;
    }
    const E& getValue() const                        // Return current element
    {                         
        VASSERT_MSG(curr->next != NULL, "No value");
        return curr->next->element;
    }
};


// Edge class for Adjacency List graph representation 邻接表表示的图结构的边类型。
class Edge 
{
    int     vert;
    float   wt;
public:
    Edge() { vert = -1; wt = -1; }
    Edge(int v, float w) { vert = v; wt = w; }
    int vertex() { return vert; }
    float weight() { return wt; }
};

class Graphl : public Graph 
{
private:
    List<Edge>** vertex;                                   // List headers
    int numVertex, numEdge;                                // Number of vertices, edges
    int *mark;                                             // Pointer to mark array

public:
    Graphl(int numVert)
    { Init(numVert); } 

    ~Graphl()                                              // Destructor
    {                                       
        delete [] mark;                                    // Return dynamically allocated memory
        for (int i=0; i<numVertex; i++) delete [] vertex[i];
        delete [] vertex;
    }
    void Init(int n) 
    {
        int i;
        numVertex = n;
        numEdge = 0;
        mark = new int[n];                                  // Initialize mark array
        for (i=0; i<numVertex; i++) mark[i] = 0;
        // Create and initialize adjacency lists
        vertex = (List<Edge>**) new List<Edge>*[numVertex];
        for (i=0; i<numVertex; i++)
            vertex[i] = new LList<Edge>();
    }
    int n() { return numVertex; }                            // Number of vertices
    int e() { return numEdge; }                              // Number of edges
    int first(int v)                                         // Return first neighbor of "v"
    {                                       
        if (vertex[v]->length() == 0)
            return numVertex;                                // No neighbor
        vertex[v]->moveToStart();
        Edge it = vertex[v]->getValue();
        return it.vertex();
    }
    // Get v’s next neighbor after w
    int next(int v, int w) 
    {
        Edge it;
        if (isEdge(v, w)) 
        {
            if ((vertex[v]->currPos()+1) < vertex[v]->length()) 
            {
                vertex[v]->next();
                it = vertex[v]->getValue();
                return it.vertex();
            }
        }
        return n();                                           // No neighbor
    }

    // Set edge (i, j) to "weight"
    void setEdge(int i, int j, float weight) 
    { 
        VASSERT_MSG( weight>0, "May not set weight to 0" );
        Edge currEdge(j, weight);
        if (isEdge(i, j))                                      // Edge already exists in graph
        {                                  
            vertex[i]->remove();
            vertex[i]->insert(currEdge);
        }
        else                                                  // Keep neighbors sorted by vertex index
        {                                                
            numEdge++;
            for (vertex[i]->moveToStart();
                vertex[i]->currPos() < vertex[i]->length();
                vertex[i]->next()) 
            {
                Edge temp = vertex[i]->getValue();
                if (temp.vertex() > j) break;
            }
            vertex[i]->insert(currEdge);
        }
    }
    void delEdge(int i, int j)                               // Delete edge (i, j)
    {                             
        if (isEdge(i,j)) 
        {
            vertex[i]->remove();
            numEdge--;
        }
    }
    bool isEdge(int i, int j)                                // Is (i,j) an edge?
    {                              
        Edge it;
        for (vertex[i]->moveToStart();
            vertex[i]->currPos() < vertex[i]->length();
            vertex[i]->next())                               // Check whole list
        {                             
            Edge temp = vertex[i]->getValue();
            if (temp.vertex() == j) return true;
        }
        return false;
    }
    float weight(int i, int j)                               // Return weight of (i, j)
    {                             
        Edge curr;
        if (isEdge(i, j)) 
        {
            curr = vertex[i]->getValue();
            return curr.weight();
        }
        else return 0;
    }
    int getMark(int v) { return mark[v]; }
    void setMark(int v, int val) { mark[v] = val; }
};

//define priority
class DDComp 
{
public:
    static bool prior(DijkElem x, DijkElem y)
    { return x.distance < y.distance; }
};

// Swap two elements in a generic array
template<typename E>
inline void swap(E A[], int i, int j) 
{
    E temp = A[i];
    A[i] = A[j];
    A[j] = temp;
}

// Heap class
template <typename E, typename Comp> 
class heap 
{
private:
    E* Heap;                                                       // Pointer to the heap array
    int maxsize;                                                   // Maximum size of the heap
    int n;                                                         // Number of elements now in the heap
    // Helper function to put element in its correct place
    void siftdown(int pos) 
    {
        while (!isLeaf(pos))                                       // Stop if pos is a leaf
        {                                     
            int j = leftchild(pos); int rc = rightchild(pos);
            if ((rc < n) && Comp::prior(Heap[rc], Heap[j]))
                j = rc;                                            // Set j to greater child’s value
            if (Comp::prior(Heap[pos], Heap[j])) return;           // Done
            swap(Heap, pos, j);
            pos = j;                                              // Move down
        }
    }
public:
    heap(E* h, int num, int max)                                  // Constructor
    { Heap = h; n = num; maxsize = max; buildHeap(); }
    int size() const                                              // Return current heap size
    { return n; }
    bool isLeaf(int pos) const                                    // True if pos is a leaf
    { return (pos >= n/2) && (pos < n); }
    int leftchild(int pos) const
    { return 2*pos + 1; }                                         // Return leftchild position
    int rightchild(int pos) const
    { return 2*pos + 2; }                                         // Return rightchild position
    int parent(int pos) const                                     // Return parent position
    { return (pos-1)/2; }
    void buildHeap()                                              // Heapify contents of Heap
    { for (int i=n/2-1; i>=0; i--) siftdown(i); }
    // Insert "it" into the heap
    void insert(const E& it) 
    {
        VASSERT_MSG(n < maxsize, "Heap is full");
        int curr = n++;
        Heap[curr] = it;                                          // Start at end of heap
        // Now sift up until curr’s parent > curr
        while ((curr!=0) && (Comp::prior(Heap[curr], Heap[parent(curr)]))) 
        {
            swap(Heap, curr, parent(curr));
            curr = parent(curr);
        }
    }

    // Remove first value
    E removefirst() 
    {
        VASSERT_MSG (n > 0, "Heap is empty");
        swap(Heap, 0, --n);                                       // Swap first with last value
        if (n != 0) siftdown(0);                                  // Siftdown new root val
        return Heap[n];                                           // Return deleted value
    }
    // Remove and return element at specified position
    E remove(int pos) 
    {
        VASSERT_MSG((pos >= 0) && (pos < n), "Bad position");
        if (pos == (n-1)) n--;                                    // Last element, no work to do
        else
        {
            swap(Heap, pos, --n);                                 // Swap with last value
            while ((pos != 0) && (Comp::prior(Heap[pos], Heap[parent(pos)]))) 
            {
                swap(Heap, pos, parent(pos));                 // Push up large key
                pos = parent(pos);
            }
            if (n != 0) siftdown(pos);                            // Push down small key
        }
        return Heap[n];
    }
};