module;

#include <ctime>
#include <stdexcept>
#include <vector>
#include <string>
#include <memory>
#include <iostream>
#include <iomanip>

import entities;
import viewhelper;
import orderservice;
import authview;
import movieservice;
import screeningservice;
import movieview;

export module ticketview;

export class TicketView {
private:
    MovieService& _movieService;
    ScreeningService& _screeningService;
    OrderService& _orderService;

    MovieUptr selectMovie();
    ScreeningUptr selectScreening(int movieId, const std::vector<ScreeningUptr>& screenings);
    std::vector<int> selectSeats(int screeningId);
    void displaySeatLayout(const std::vector<ScreeningSeatUptr>& seats);
    OrderUptr confirmOrder(const User& currentUser, int ScreeningId, const std::vector<int>& selectedSeats, const Movie& movie, const Screening& screening);
    bool processPayment(Order& order);

public:
    TicketView(MovieService& movieService, ScreeningService& screeningService, OrderService& orderService);

    void buyTicket(const User& currentUser);
};