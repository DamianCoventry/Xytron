#ifndef INCLUDED_HIGHSCORES
#define INCLUDED_HIGHSCORES

#include <string>
#include <vector>

class Highscore
{
    friend class Highscores;
public:
    Highscore() : level_(0), score_(0) {}
    const std::string& Name() const         { return name_; }
    const std::string& DateTime() const     { return date_time_; }
    unsigned int Level() const              { return level_; }
    unsigned long Score() const             { return score_; }
private:
    std::string name_;
    std::string date_time_;
    unsigned int level_;
    unsigned long score_;
};

class Highscores
{
public:
    static const int NUM_HIGHSCORES = 10;

    typedef std::vector<Highscore> HighscoreTable;
    typedef HighscoreTable::const_iterator Iterator;

public:
    Highscores() { Reset(); }

    Iterator Begin() const  { return highscore_table_.begin(); }
    Iterator End() const    { return highscore_table_.end(); }

    bool Load(const std::string& filename);
    bool Save(const std::string& filename);

    void Reset();
    int Insert(const std::string& name, unsigned int level, unsigned long score);
    bool IsHighscore(unsigned long score) const;

private:
    HighscoreTable highscore_table_;
};

#endif  // INCLUDED_HIGHSCORES
