
namespace _Wolframe {
namespace langbind {

// Output filter
class OutputFilter
    :public FilterBase
{
public:
    // State of the input filter
    enum State
    {
        Open,           //< normal input processing
        EndOfBuffer,    //< end of buffer reached
        Error           //< have to stop with an error
    };

    // Default constructor
    OutputFilter(
        const char* name_,
        const ContentFilterAttributes* attr_=0);

    // Copy constructor
    OutputFilter( const OutputFilter& o);

    // Destructor
    virtual ~OutputFilter(){}

    // Get a self copy
    virtual OutputFilter* copy() const=0;

    // Print the follow element to the buffer
    virtual bool print(
        ElementType type,
        const void* element,
        std::size_t elementsize)=0;

    // Set the document meta data.
    void setMetaData( const types::DocMetaData& md);

    // Get a reference to the document meta data.
    const types::DocMetaData* getMetaData() const;

    // Get the current state
    State state() const;

    // Set output filter state with error message
    void setState( State s, const char* msg=0);

protected:
    std::size_t write( const void* dt, std::size_t dtsize);
};

///\typedef OutputFilterR
// Shared output filter reference
typedef types::SharedReference<OutputFilter> OutputFilterR;

}}//namespace
#endif


