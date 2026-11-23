# Coda-C
Object Oriented Programming in C

## ⭐️ The Coda-C Library ⭐️

This library provides the tools to write OOP directly in C, without any pre-processing.
A command line tool for header generation is included assist with constructing headers.

## 🔵 Features:

* Retain counted objects
* Dynamic Array and Dictionary
* Virtual functions obj_ & super_
* String constant objects Os("string")
* Header Generator included: $ coda-h
* Includes usage information in PDF format.

## 🟤 Requirements

* gcc or clang

## 🟢 Building

You need a modern C compiler supporting '-std=gnu17'. A Makefile is provided to install the dynamic library, header, and 'coda-h' in /usr/local or /usr for Linux.
Note:  For Linux, "sudo ldconfig" should be run after "sudo make install".
Note:  Windows is not officially supported.

## 🟣 Examples

Compile examples with "cc FILE.c -lcoda-c" after installing the library.

## 🟣 Limits

* Objects are limited to 8 Meg
* Retain counts are limited to 64K
* Plain C strings are not objects, use Os("string") instead

