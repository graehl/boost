/*=============================================================================
    Spirit V1.5.2
    Copyright (c) 1998-2003 Joel de Guzman
    http://spirit.sourceforge.net/

    Permission to copy, use, modify, sell and distribute this software is
    granted provided this copyright notice appears in all copies. This
    software is provided "as is" without express or implied warranty, and
    with no claim as to its suitability for any purpose.
=============================================================================*/
#if !defined(BOOST_SPIRIT_PARSER_HPP)
#define BOOST_SPIRIT_PARSER_HPP

///////////////////////////////////////////////////////////////////////////////
#include "boost/config.hpp"
#include "boost/spirit/core/scanner/scanner.hpp"

#if defined(BOOST_MSVC)
#include "boost/spirit/core/impl/msvc.hpp"
#endif

#if defined(BOOST_MSVC) && (BOOST_MSVC <= 1300)
#define BOOST_SPIRIT_PARSER_RESULT_ARGS typename ScannerT, typename Pizza = nil_t
#else
#define BOOST_SPIRIT_PARSER_RESULT_ARGS typename ScannerT
#endif

///////////////////////////////////////////////////////////////////////////////
namespace boost
{
    namespace spirit
    {

    template <typename ParserT, typename ActionT>
    class action;   //  forward declaration

    ///////////////////////////////////////////////////////////////////////////
    //
    //  Parser categories
    //
    //      Helper template classes to distinguish different types of
    //      parsers. The following categories are the most generic. More
    //      specific types may inherit from these. Each parser has a typedef
    //      parser_category_t that defines its category. By default, if one
    //      is not specified, it will inherit from the base parser class
    //      which typedefs its parser_category_t as plain_parser_category.
    //
    //          - plain parser has nothing special
    //          - binary parser has subject a and b (e.g. alternative)
    //          - unary parser has single subject  (e.g. kleene star)
    //          - action parser has an attached action parser
    //
    ///////////////////////////////////////////////////////////////////////////
    struct plain_parser_category {};
    struct binary_parser_category       : plain_parser_category {};
    struct unary_parser_category        : plain_parser_category {};
    struct action_parser_category       : unary_parser_category {};

    ///////////////////////////////////////////////////////////////////////////
    //
    //  parser_result metafunction
    //
    //      Given a scanner type ScannerT and a parser type ParserT, the
    //      parser_result metafunction provides the actual result of the
    //      parser.
    //
    //  Usage:
    //
    //      typename parser_result<ParserT, ScannerT>::type
    //
    ///////////////////////////////////////////////////////////////////////////
    #if defined(BOOST_MSVC) && (BOOST_MSVC <= 1300)

    BOOST_SPIRIT_DEPENDENT_TEMPLATE_WRAPPER(parser_result_wrapper, result);

    //////////////////////////////////
    template <typename ParserT, typename ScannerT>
    struct parser_result
    {
        typedef typename impl::parser_result_wrapper<ParserT>
            ::template result_<ScannerT>::type type;
    };

    #else

    //////////////////////////////////
    template <typename ParserT, typename ScannerT>
    struct parser_result
    {
        typedef typename ParserT::template result<ScannerT>::type type;
    };

    #endif

    ///////////////////////////////////////////////////////////////////////////
    //
    //  parser class
    //
    //      This class is a protocol base class for all parsers. This is
    //      essentially an interface contract. The parser class does not
    //      really know how to parse anything but instead relies on the
    //      template parameter DerivedT (which obviously is assumed to be a
    //      subclass) to do the actual parsing.
    //
    //      Concrete sub-classes inheriting from parser must have a
    //      corresponding member function parse(...) compatible with the
    //      conceptual Interface:
    //
    //          template <typename ScannerT>
    //          RT parse(ScannerT const& scan) const;
    //
    //      where RT is the desired return type of the parser and ScannerT
    //      scan is the scanner (see scanner.hpp).
    //
    //      Concrete sub-classes inheriting from parser in most cases need to
    //      have a nested meta-function result that returns the result type
    //      of the parser's parse member function, given a scanner type. The
    //      meta- function has the form:
    //
    //          template <typename ScannerT>
    //          struct result
    //          {
    //              typedef RT type;
    //          };
    //
    //      where RT is the desired return type of the parser. This is
    //      usually, but not always, dependent on the template parameter
    //      ScannerT. If a parser does not supply a result metafunction, a
    //      default is provided by the base parser class.
    //
    //      The parser's derived() member function returns a reference to the
    //      parser as its derived object.
    //
    //      An operator[] is provided. The operator returns a semantic action
    //      handler (see actions.hpp).
    //
    //      Each parser has a typedef embed_t. This typedef specifies how a
    //      parser is embedded in a composite (see composite.hpp). By
    //      default, if one is not specified, the parser will be embedded by
    //      value. That is, a copy of the parser is placed as a member
    //      variable of the composite. Most parsers are embedded by value. In
    //      certain situations however, this is not desirable or possible.
    //
    ///////////////////////////////////////////////////////////////////////////
    template <typename DerivedT>
    struct parser
    {
        typedef DerivedT                embed_t;
        typedef DerivedT                derived_t;
        typedef plain_parser_category   parser_category_t;

        template <BOOST_SPIRIT_PARSER_RESULT_ARGS>
        struct result
        {
            typedef typename match_result<ScannerT, nil_t>::type type;
        };

        DerivedT& derived()
        { return *static_cast<DerivedT*>(this); }

        DerivedT const& derived() const
        { return *static_cast<DerivedT const*>(this); }

        template <typename ActionT>
        action<DerivedT, ActionT>
        operator[](ActionT const& actor) const
        { return action<DerivedT, ActionT>(derived(), actor); }
    };

    ///////////////////////////////////////////////////////////////////////////
    //
    //  parse_info
    //
    //      Results returned by the free parse functions:
    //
    //      stop:   points to the final parse position (i.e parsing
    //              processed the input up to this point).
    //
    //      hit:    true if parsing is successful. This may be full:
    //              the parser consumed all the input, or partial:
    //              the parser consumed only a portion of the input.
    //
    //      full:   true when we have a full hit (i.e the parser
    //              consumed all the input.
    //
    //      length: The number of characters consumed by the parser.
    //              This is valid only if we have a successful hit
    //              (either partial or full). A negative value means
    //              that the hit is unsucessful.
    //
    ///////////////////////////////////////////////////////////////////////////
    template <typename IteratorT = char const*>
    struct parse_info
    {
        IteratorT   stop;
        bool        hit;
        bool        full;
        unsigned    length;

        parse_info(
            IteratorT const& stop_ = IteratorT(),
            bool hit_ = false,
            bool full_ = false,
            unsigned length_ = 0)
        : stop(stop_)
        , hit(hit_)
        , full(full_)
        , length(length_) {}

        template <typename ParseInfoT>
        parse_info(ParseInfoT const& pi)
        : stop(pi.stop)
        , hit(pi.hit)
        , full(pi.full)
        , length(pi.length) {}
    };

    ///////////////////////////////////////////////////////////////////////////
    //
    //  Generic parse function
    //
    ///////////////////////////////////////////////////////////////////////////
    template <typename IteratorT, typename DerivedT>
    parse_info<IteratorT>
    parse(
        IteratorT const&        first,
        IteratorT const&        last,
        parser<DerivedT> const& p);

    ///////////////////////////////////////////////////////////////////////////
    //
    //  Parse function for null terminated strings
    //
    ///////////////////////////////////////////////////////////////////////////
    template <typename CharT, typename DerivedT>
    parse_info<CharT const*>
    parse(
        CharT const*            str,
        parser<DerivedT> const& p);

    } // namespace spirit
} // namespace boost

#endif

#include "boost/spirit/core/impl/parser.ipp"
#undef BOOST_SPIRIT_PARSER_RESULT_ARGS
