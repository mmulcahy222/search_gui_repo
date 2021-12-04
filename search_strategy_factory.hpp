#include <iostream>
#include <Windows.h>
#include <string.h>
#include <shellapi.h>


using namespace std;


void open_url_in_browser(string url)
{
    ShellExecute(NULL, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
}



//strategy pattern for searching websites
class SearchStrategy
{
public:
    virtual void search(string) = 0;
};
class YoutubeSearch : public SearchStrategy
{
public:
    void search(string query)
    {
        open_url_in_browser("https://www.youtube.com/results?search_query=" + query);
    }
};
class SpotifySearch : public SearchStrategy
{
public:
    void search(string query)
    {
        open_url_in_browser("https://open.spotify.com/search/" + query);
    }
};
class GoogleSearch : public SearchStrategy
{
public:
    void search(string query)
    {
        open_url_in_browser("https://www.google.com/search?q=" + query);
    }
};
class DiscogsSearch : public SearchStrategy
{
public:
    void search(string query)
    {
        open_url_in_browser("https://www.discogs.com/search/?q=" + query);   
    }
};

//factory pattern to return strategies, returning a static variable to avoid creating multiple instances
class SearchStrategyFactory
{
private:
    YoutubeSearch *youtube;
    GoogleSearch *google;
    SpotifySearch *spotify;
    DiscogsSearch *discogs;

public:
    SearchStrategy *getSearchStrategy(string search)
    {
        if (search == "youtube")
            return youtube == nullptr ? youtube = new YoutubeSearch() : youtube;
        else if (search == "google")
            return google == nullptr ? google = new GoogleSearch() : google;
        else if (search == "spotify")
            return spotify == nullptr ? spotify = new SpotifySearch() : spotify;
        else if (search == "discogs")
            return discogs == nullptr ? discogs = new DiscogsSearch() : discogs;
        return nullptr;
    }
};

//context class that contains the strategy
class Search
{
private:
    SearchStrategy *strategy;
    SearchStrategyFactory *factory;

public:
    Search()
    {
        this->factory = new SearchStrategyFactory();
    }
    Search(string search)
    {
        this->factory = new SearchStrategyFactory();
        this->strategy = factory->getSearchStrategy(search);
    }
    void search(string query)
    {
        strategy->search(query);
    }
    void setStrategy(string search)
    {
        strategy = factory->getSearchStrategy(search);
    }
};

