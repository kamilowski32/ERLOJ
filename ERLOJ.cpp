#include <iostream>
#include <map>
#include <vector>
#include <set>
#include <algorithm>
#include <string>
#include <ranges>
#include <list>
#define ts to_string
using namespace std;

bool sortBySecond(const pair<short, short>& a, const pair<short, short>& b) //function to help sort buses by second element
{
    return a.second < b.second;
}

class TimetableFacade //class showing buses, which stops at that one bus stop
{
public:
    TimetableFacade()
    {
        announcement = "";
    }
    void setInfo(string info)
    {
        announcement = info;
    }
    void removeInfo()
    {
        announcement = "";
    }
    string getInfo() const
    {
        return announcement;
    }
    void addBus(short line, short timeInMinutes) //adds bus from line number 'line' which arrives at timeInMinutes
    {
        for (auto x : Table)
        {
            if (x.first == line && x.second == timeInMinutes)
            {
                return;
            }
        }
        Table.insert(pair<short, short>(line, timeInMinutes));
    }
    vector<short> getAllBusLines() //shows all bus lines stopping at this stop
    {
        vector<short> helper;
        for (auto it = Table.begin(), end = Table.end(); it != end; it = Table.upper_bound(it->first))
        {
            helper.push_back(it->first);
        }
        return helper;

    }
    vector<short> getAllBusesTimes(short line) //shows times that bus from line 'line' stops there
    {
        vector<short> helper;
        auto itr1 = Table.lower_bound(line);
        auto itr2 = Table.upper_bound(line);

        while (itr1 != itr2)
        {
            if (itr1->first == line)
                helper.push_back(itr1->second);
            itr1++;
        }
        return helper;
    }
    short getNextBusTime(short line, short actualTimeInMinutes) //shows when next bus from line 'line' arrives if we have now actualTimeInMinutes
    {
        vector<short> helper = getAllBusesTimes(line);
        if (helper.size() > 0)
        {
            for (int i = 0; i < helper.size(); i++)
            {
                helper[i] -= actualTimeInMinutes;
                if (helper[i] < 0)
                    helper[i] += 60;
            }
            sort(helper.begin(), helper.end());
            return helper[0];
        }
        return -1; 
    }
    void addBusInterval(short line, short firstTime, short interval, short count) //adds many buses from one line arriving in intervals
    {
        for (int i = 0; i < count; i++)
        {
            addBus(line, firstTime);
            firstTime += interval;
            firstTime = firstTime % 60;
        }
    }
    void removeBus(short line, short timeInMinutes) //allows to remove bus
    {
        auto iter = findByKeyAndValue(line, timeInMinutes);
        if (iter != Table.end())
            Table.erase(iter);
    }
    string getDisplayString(short actualTimeInMinutes, short displayLines) //shows us 'displayLines' number of lines which arrive next, first we want to see first bus arriving from each line and them first buses arriving from each line
    {
        if (announcement != "")
            displayLines--;
        if (displayLines < 3 || displayLines < getAllBusLines().size())
            return "ERROR";
        string returned = "";
        returned = returned + ts(day) + "-" + ts(month) + "-" + ts(year) + "\n";
        multimap<short, short> clone = Table;
        for (auto& item : Table) {
            item.second -= actualTimeInMinutes;
            if (item.second < 0)
                item.second += 60;
        }
        vector<short> helper = getAllBusLines();
        vector<vector<short>> helper2;
        vector<pair<short, short>> helper3;
        int iter = 0, min;
        for (int i : helper)
        {
            helper2.push_back(getAllBusesTimes(i));
            min = *min_element(helper2[iter].begin(), helper2[iter].end());
            iter++;
            for (auto& item : clone)
            {
                if (item.first == i && item.second == min)
                {
                    item.second += 60;
                }
            }
            helper3.push_back(make_pair(i, min));
        }
        sort(helper3.begin(), helper3.end(), sortBySecond);
        for (auto i : helper3)
        {
            returned = returned + " " + ts(i.first) + " " + ts(i.second);
        }
        if (displayLines > helper.size())
        {
            vector<pair<short, short>> vec;
            for (auto const iterator : clone)
                vec.push_back(make_pair(iterator.first, iterator.second));
            sort(vec.begin(), vec.end(), sortBySecond);
            for (int i = 0; i < displayLines - helper.size(); i++)
            {
                pair<short, short> min = vec[0];
                vec.erase(vec.begin());
                vec.push_back(make_pair(min.first, min.second + 60));
                returned += " " + ts(min.first) + " " + ts(min.second);
            }
        }
        returned = returned + "\n" + announcement + "\n";
        return returned;
    }
    void setActualDate(short _day, short _month, short _year) //allows us to set actual date
    {
        day = _day;
        month = _month;
        year = _year;
    }
    ~TimetableFacade()
    {

    }
private:
    multimap<short, short>::const_iterator findByKeyAndValue(short a, short b)
    {
        auto range = Table.equal_range(a);
        return find_if(range.first, range.second, [b](const auto& p) {return p.second == b;  });
    }
    multimap<short, short> Table;
    string announcement;
    short day, month, year;
};

int main()
{
    TimetableFacade facade;
    facade.addBus(110,12);
    facade.addBus(111, 12);
    facade.addBus(112, 10);
    facade.addBusInterval(112, 13, 47, 40);
    facade.removeBus(110, 12);
    auto x = facade.getAllBusLines();
    for (auto i : x)
    {
        cout << i << endl;
    }
    cout << facade.getNextBusTime(112, 12);
    cout << "---------------------------------------";
    facade.setActualDate(11, 13, 1312);
    facade.setInfo("LINE 120 IS NOT RUNNING");
    cout << facade.getDisplayString(25, 85);
}