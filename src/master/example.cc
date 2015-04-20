#include <libcouchbase/couchbase++.h>
#include <libcouchbase/couchbase++/views.h>

using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::map;
using std::vector;

int main(int argc, const char **argv)
{
    //! Connect to the client
    string connstr(argc > 1 ? argv[1] : "couchbase://92.222.84.96/default");
    Couchbase::Client h(connstr);
    Couchbase::Status rv = h.connect();
    if (!rv.success()) {
        cout << "Couldn't connect to '" << connstr << "'. "
                << "Reason: " << rv << endl;
        exit(EXIT_FAILURE);
    }

    //! Store item.
    Couchbase::UpsertOperation scmd("foo",
        "{ \"v\": 100.1, \"list\": [1,2,3,4,5,6,7], \"talk\": \"About Foo.\" }");
    auto sres = scmd.run(h);
    cout << "Got status for store. Cas=" << std::hex << sres.cas() << endl;

    //! don't use implicit op
    sres = h.upsert(scmd);

    //! Use a command to retrieve an item
    Couchbase::GetOperation cmd("foo");
    cmd.run(h);
    //! The response object can be returned via run() or via response()
    auto res = cmd.response();
    cout << "Got value: " << res.value() << std::endl;

    cmd.key("non-exist-key");
    cmd.run(h);
    cout << "Got status for non-existent key: " << cmd.response().status() << endl;

    //! Use batch contexts to perform "bulk" operations
    Couchbase::BatchContext ctx(h);
    for (size_t ii = 0; ii < 10; ii++) {
        scmd.schedule(ctx);
    }
    ctx.submit();
    h.wait();

    //! Store a bunch of items, one at a time (does not use batching)
    Couchbase::UpsertOperation("foo", "FOOVALUE").run(h);
    Couchbase::UpsertOperation("bar", "BARVALUE").run(h);
    Couchbase::UpsertOperation("baz", "BAZVALUE").run(h);

    //! Use durability requirements
    Couchbase::UpsertOperation endureOp("toEndure", "toEndure");
    endureOp.run(h);
    cout << "Endure status: " << Couchbase::DurabilityOperation(endureOp).run(h).status() << endl;

    //! Reset the context.
    ctx.reset();

    //! Use shorthand for "getting" an item.
    ctx.get("foo");
    ctx.get("bar");
    ctx.get("baz");
    ctx.submit(); // Schedule the pipeline

    // Wait for them all!
    h.wait();

    cout << "Value for foo is " << ctx["foo"].value() << endl;
    cout << "Value for bar is " << ctx["bar"].value() << endl;
    cout << "Value for baz is " << ctx["baz"].value() << endl;

    //! Remove the items we just created
    Couchbase::RemoveOperation("foo").run(h);
    Couchbase::RemoveOperation("bar").run(h);
    Couchbase::RemoveOperation("baz").run(h);

    Couchbase::Status status;
    Couchbase::ViewCommand vCmd("beer", "brewery_beers");
    vCmd.include_docs();
    vCmd.add_option("limit", 10);
    vCmd.add_option("skip", 10);
    vCmd.add_option("descending", true);
    cout << "using options: " << vCmd.get_options() << endl;

    Couchbase::ViewQuery query(h, vCmd, status);
    if (!status) {
        cerr << "Error with view command: " << status << endl;
    }

    size_t numRows = 0;
    for (auto ii = query.begin(); ii != query.end(); ii++) {
            cout << "Key: " << ii->key() << endl;
            cout << "Value: " << ii->value() << endl;
            cout << "DocID: " << ii->docid() << endl;

            if (ii->has_document()) {
                std::string value;
                ii->document().value(value);
                cout << "Document: " << value << endl;
            } else {
                cout << "NO DOCUMENT!" << endl;
            }
        numRows ++;
    }
    cout << "Got: " << std::dec << numRows << " rows" << endl;
    if (!query.status()) {
        cerr << "Problem with query: " << query.status() << endl;
    }
    return 0;
}
