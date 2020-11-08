#ifndef RCP_ERROR_LISTENER_H
#define RCP_ERROR_LISTENER_H

namespace rcp {

    class ParsingErrorListener
    {
    public:
        virtual void parsingError() = 0;
    };

}

#endif // RCP_ERROR_LISTENER_H
