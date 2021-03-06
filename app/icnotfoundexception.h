#ifndef ICNOTFOUNDEXCEPTION_H
#define ICNOTFOUNDEXCEPTION_H

#include <stdexcept>

class IC;
class ICNotFoundException : public std::runtime_error
{
    IC *m_ic;

public:
    ICNotFoundException(const std::string &message, IC *ic);
    IC *getIC() const;
};

#endif /* ICNOTFOUNDEXCEPTION_H */
