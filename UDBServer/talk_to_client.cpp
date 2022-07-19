#include "talk_to_client.h"
#include <iostream>

io_service service;

typedef boost::shared_ptr<talk_to_client> client_ptr;
typedef std::vector<client_ptr> array;
array clients;

boost::recursive_mutex cs;

talk_to_client::talk_to_client() : sock_(service), started_(false), already_read_(0)
{
    last_ping = microsec_clock::local_time();
}

std::string talk_to_client::username() const
{
    return username_;
}

void talk_to_client::answer_to_client()
{
    try {
        read_request();
        process_request();
    }
    catch ( boost::system::system_error&) {
        stop();
    }
    if ( timed_out()) {
        stop();
        std::cout << "stopping " << username_ << " - no ping in time" << std::endl;
    }
}

void talk_to_client::set_clients_changed()
{
    clients_changed_ = true;
}

ip::tcp::socket &talk_to_client::sock()
{
    return sock_;
}

bool talk_to_client::timed_out() const
{
    ptime now = microsec_clock::local_time();
    long long ms = (now - last_ping).total_milliseconds();
    return ms > 5000 ;
}

void talk_to_client::stop()
{
    // close client connection
    boost::system::error_code err;
    sock_.close(err);
}

void talk_to_client::read_request()
{
    if ( sock_.available())
        already_read_ += sock_.read_some(buffer(buff_ + already_read_, max_msg - already_read_));
}

void talk_to_client::process_request()
{
    bool found_enter = std::find(buff_, buff_ + already_read_, '\n') < buff_ + already_read_;
    if ( !found_enter)
        return; // message is not full
    // process the msg
    last_ping = microsec_clock::local_time();
    size_t pos = std::find(buff_, buff_ + already_read_, '\n') - buff_;
    std::string msg(buff_, pos);
    std::copy(buff_ + already_read_, buff_ + max_msg, buff_);
    already_read_ -= pos + 1;

    if ( msg.find("login ") == 0) on_login(msg);
    else if ( msg.find("ping") == 0) on_ping();
    else if ( msg.find("ask_clients") == 0) on_clients();
    else std::cerr << "invalid msg " << msg << std::endl;
}

void talk_to_client::on_login(const std::string &msg)
{
    std::istringstream in(msg);
    in >> username_ >> username_;
    std::cout << username_ << " logged in" << std::endl;
    write("login ok\n");
    update_clients_changed();
}

void talk_to_client::on_ping()
{
    write(clients_changed_ ? "ping client_list_changed\n" : "ping ok\n");
    clients_changed_ = false;
}

void talk_to_client::on_clients()
{
    std::string msg;
    {boost::recursive_mutex::scoped_lock lk(cs);
        for( array::const_iterator b = clients.begin(), e = clients.end() ; b != e; ++b)
            msg += (*b)->username() + " ";
    }
    write("clients " + msg + "\n");
}

void talk_to_client::write(const std::string &msg)
{
    sock_.write_some(buffer(msg));
}

void update_clients_changed()
{
    boost::recursive_mutex::scoped_lock lk(cs);
    for(array::iterator b = clients.begin(), e = clients.end(); b != e; ++b)
        (*b)->set_clients_changed();
}



void accept_thread()
{
    ip::tcp::acceptor acceptor(service, ip::tcp::endpoint(ip::tcp::v4(), 8004));
    while ( true) {
        client_ptr new_(new talk_to_client);
        acceptor.accept(new_->sock());

        boost::recursive_mutex::scoped_lock lk(cs);
        clients.push_back(new_);
    }
}

void handle_clients_thread()
{
    while ( true) {
        boost::this_thread::sleep( millisec(1));
        boost::recursive_mutex::scoped_lock lk(cs);
        for ( array::iterator b = clients.begin(), e = clients.end(); b != e; ++b)
            (*b)->answer_to_client();
        // erase clients that timed out
        clients.erase(std::remove_if(clients.begin(), clients.end(),
            boost::bind(&talk_to_client::timed_out,_1)), clients.end());
    }
}
















