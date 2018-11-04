#pragma once

#include "types.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <sstream>


using std::string;
using std::unordered_map;
using std::vector;

namespace file_mapping {
    class ConfigElement {
    };

    class Definition : public ConfigElement {
    };

    struct Person : public Definition {
        string name;
        vector<string> aliases;

        Person(string &&name, vector<string> &&aliases) : name{std::move(name)}, aliases{std::move(aliases)} {}
        Person(Person &other) = delete;
        Person(Person &&old) : name{std::move(old.name)}, aliases{std::move(old.aliases)} {}
    };

    struct Group : public Definition {
        string name;
        vector<string> mapsTo;

        Group(string &&name, vector<string> &&mapsTo) : name{std::move(name)}, mapsTo{std::move(mapsTo)} {}
        Group(Group &other) = delete;
        Group(Group &&old) : name{std::move(old.name)}, mapsTo{std::move(old.mapsTo)} {}
    };
    struct Currency : public Definition {
        string name;

        Currency(string&& name):name{name}{}
        Currency(Currency&& old): name{old.name}{}
        Currency(Currency& other) = delete;
    };

    using Value = std::pair<double, Currency>;

    using CurrencyTransformation = std::pair<Value, Value>;

    struct Transaction : ConfigElement{
        string paidBy;
        std::pair<double, string> value;
        vector<string> paidFor;

        Transaction(string &&paidBy, std::pair<double, string>&& value, vector<string>&& paidFor):paidBy{std::move(paidBy)}, value{std::move(value)}, paidFor{std::move(paidFor)} {}
        Transaction(Transaction& other) = delete;
    };
}

namespace internal {
    using person_id_t = usize;

    class PersonIDRegister {
    private:
        person_id_t last_id = static_cast<person_id_t>(-1);
        unordered_map<string, person_id_t> registry;

        void add_name(string &name, person_id_t id) {
            auto[_, success] = registry.insert({name, id});
            if (!success) {
                std::ostringstream stringStream;
                stringStream << "Person with name \"" << name << "\" is defined twice!";
                throw stringStream.str();
            }
        }

        person_id_t add_name_auto_id(string &name) {
            last_id++;
            add_name(name, last_id);
            return last_id;
        }

    public:
        void add_person(file_mapping::Person &person) {
            auto id = add_name_auto_id(person.name);
            for (auto alias : person.aliases) add_name(alias, id);
        }

        person_id_t get_id(string &name) const {
            return registry.at(name);
        }

        person_id_t get_id(file_mapping::Person &person) const {
            return get_id(person.name);
        }
    };
}