class Stats {
    mutable int count;
public:
    void increment() {
        ++count;
    }
    int get() const {
        return count;
    }
};