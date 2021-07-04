#ifndef INCLUDED_BINDINGSFILE
#define INCLUDED_BINDINGSFILE

#include <string>
#include <map>

class BindingsFile
{
public:
    typedef std::map<std::string, std::string> InputBindings;

public:
    void Load(const std::string& filename);
    void Save(const std::string& filename);

    typedef InputBindings::const_iterator Iterator;
    Iterator Begin() const  { return input_bindings_.begin(); }
    Iterator End() const    { return input_bindings_.end(); }

    void Set(const std::string& binding, const std::string& value);
    void Clear(const std::string& binding);
    void ClearAll();

private:
    InputBindings input_bindings_;
};

#endif  // INCLUDED_BINDINGSFILE
