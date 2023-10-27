#include<iostream>
#include<random>
#include<vector>
#include<algorithm>
#include<thread>
#include<Aliz/AlizChrono.hpp>
#include<atomic>

enum Suites {
  Spades,
  Clubs,
  Diamonds,
  Hearts,
};

enum Ranks
{
  Two=2,
  Three,
  Four,
  Five,
  Six,
  Seven,
  Eight,
  Nine,
  Ten,
  Jack,
  Queen,
  King,
  Ace
};

struct Card
{
  Suites suite;
  Ranks rank;
  int serial_number;
};



class Deck
{
  std::mt19937_64 &gen_;
  std::vector<Card> cards_{52};

  void initialize_() noexcept;

public:
  Deck(std::mt19937_64 &generator) noexcept;

  void shuffle() noexcept;
  std::vector<Card> const &cards() noexcept;
  void dump(std::ostream& os) const noexcept;
};


Deck::Deck(std::mt19937_64 &generator) noexcept
:gen_(generator)
{
  initialize_();
}

void Deck::shuffle() noexcept
{
  std::shuffle(cards_.begin(),cards_.end(),gen_);
}

void Deck::initialize_() noexcept
{
  assert(cards_.size()==52);
  int sn=0;
  auto itor=cards_.begin();
  for(auto s:{Spades,Clubs,Diamonds,Hearts})
  {
    for(auto r:{Ace,Two,Three,Four,Five,Six,Seven,Eight,Nine,Ten,Jack,Queen,King})
    {
      itor->suite=s;
      itor->rank=r;
      itor->serial_number=sn++;
      ++itor;

      if(itor==cards_.end())
        break;
    }
    if(itor==cards_.end())
      break;
  }
  assert(52==sn);
}

std::vector<Card> const &Deck::cards() noexcept
{
  return cards_;
}

void Deck::dump(std::ostream &os) const noexcept
{
  size_t i=0;
  for(auto const &card:cards_)
  {
    if(i && !(i%13))
      os<<std::endl;

    os<<"("<<card.suite<<":"<<card.rank<<";"<<card.serial_number<<"),\t";
    ++i;
  }
  os<<std::endl;
}


void problem_1(std::random_device::result_type seed, size_t ensembles)
{
  std::cout<<"problem_1: "<<std::endl;

  std::mt19937_64 gen(seed);
  Deck deck(gen);

  deck.dump(std::cout);

  std::cout<<"problem_1: sampling "<<ensembles<<" ensembles..."<<std::endl;
  Aliz::Stopwatch timer;

  timer.start("problem 1");
  std::vector<std::thread> threads;
  std::mutex mutex;
  size_t done{0};
  size_t total_ace_of_spades{0};
  size_t total_two_of_clubs{0};

  for(size_t i=0;i<std::thread::hardware_concurrency();++i)
  {
    threads.emplace_back([threads=std::thread::hardware_concurrency(),
                          idx=i,
                          seed=gen(),
                          &mutex,
                          &done,
                          &total_ace_of_spades,
                          &total_two_of_clubs,
                          ensembles](){
      size_t iterations=ensembles/threads;
      if(0==idx)
        iterations += ensembles%threads;

      std::mt19937_64 gen(seed);
      Deck deck(gen);
      size_t ace_of_spades{0};
      size_t two_of_clubs{0};

      for(size_t i=0;i<iterations;++i)
      {
        deck.shuffle();
        auto const &cards=deck.cards();
        auto card=std::find_if(cards.cbegin(),cards.cend(),[](const Card &theCard){
          return Ace==theCard.rank;
        });
        assert(card!=cards.cend());
        ++card;
        if(card!=cards.cend())
        {
          if(Ace==card->rank && Spades==card->suite)
            ++ace_of_spades;
          else if(Two==card->rank && Clubs==card->suite)
            ++two_of_clubs;
        }

      } // for

      do
      {
        std::lock_guard<std::mutex> lock(mutex);
        done += iterations;
        total_ace_of_spades += ace_of_spades;
        total_two_of_clubs += two_of_clubs;
      }
      while(0);
    });
  }

  for(auto &t:threads)
  {
    if(t.joinable())
      t.join();
  }
  std::cout<<"problem_1: sampling "<<ensembles<<" ensembles... done."<<std::endl;

  timer.stop();
  std::cout<<timer<<std::endl;

  double ace_ratio=1.0*total_ace_of_spades / done;
  double two_ratio=1.0*total_two_of_clubs / done;
  std::cout<<"ace of spades = "<<total_ace_of_spades<<", ratio = "<<ace_ratio<<", 1/ratio = "<<(1.0/ace_ratio)<<std::endl;
  std::cout<<"two of clubs = "<<total_two_of_clubs<<", ratio = "<<two_ratio<<", 1/ratio = "<<(1.0/two_ratio)<<std::endl;

  std::cout<<"problem_1: done."<<std::endl;
}

void problem_2(std::random_device::result_type seed, size_t ensembles) {
  std::cout<<"problem_2: sampling "<<ensembles<<" ensembles..."<<std::endl;

  std::mt19937_64 gen(seed);
  Deck deck(gen);

  Aliz::Stopwatch timer;

  timer.start("problem 2");

  std::vector<std::thread> threads;
  std::atomic_uint64_t total_same_spots{0};
  std::atomic_uint64_t total_experiments{0};
  size_t total_threads=std::thread::hardware_concurrency();

  for(size_t i=0;i<total_threads;++i)
  {
    threads.emplace_back([idx=i,
                          threads=total_threads,
                          &total_same_spots,
                          &total_experiments,
                          seed=gen(),
                          ensembles](){
      size_t hits{0};
      size_t trials=ensembles/threads;
      if(!idx)
        trials += ensembles%threads;
      std::mt19937_64 gen(seed);

      Deck deck(gen);
      auto const &cards=deck.cards();

      for(size_t i=0;i<trials;++i)
      {
        deck.shuffle();
        for(size_t i=0;i<52;++i)
        {
          if(i==cards[i].serial_number)
            ++hits;
        }
      }

      total_experiments += 52*trials;
      total_same_spots += hits;
    });
  }

  for(auto &t:threads)
  {
    if(t.joinable())
      t.join();
  }

  timer.stop();
  std::cout<<timer<<std::endl;
  std::cout<<"problem_2: sampling "<<ensembles<<" ensembles... done."<<std::endl;

  double hitRatio=1.0*total_same_spots/total_experiments;
  std::cout<<"total_same_spots = "<<total_same_spots<<", total_experiments = "<<total_experiments<<", ratio = "<<hitRatio<<", 1/ratio = "<<(1.0/hitRatio)<<std::endl;

  std::cout<<"problem_2: done."<<std::endl;
}

int main()
{
  std::random_device rand;

  //problem_1(rand(),0x1ffffff);
  problem_2(rand(),0x1ffffff);
  return 0;
}
