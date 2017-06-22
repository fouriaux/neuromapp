#ifndef NEUROMAPP_BLOCK
#define NEUROMAPP_BLOCK

#include <string>
#include <memory> // POSIX, size_t is inside
#include <functional>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <limits>
#include <type_traits>
#include <cctype>
#include <cassert>

#include "type_definition.h"
#include "allocator.h" // that's right, this is another way to tie up files in different locations.
#include "compressor.h"
#include "exception.h"

namespace neuromapp {

    // other allocator nothing special col is not modify
    template <class Allocator>
        inline std::size_t resize_helper(std::size_t n, std::size_t sizeof_T) {
            return n;
        }

    // specific to align allocator
    template <>
        inline std::size_t resize_helper<align>(std::size_t n, std::size_t sizeof_T) {
            return align::resize_policy(n, sizeof_T);
        }

    //default compress algos are zlib util
    //beginning of actual block class material
    template <class T= typename memory_policy_type::value_type, class allocator = typename memory_policy_type::block_allocator_type,class compressor = typename memory_policy_type::block_compressor_type>
        class block : public allocator, compressor {
            using allocator::allocate_policy;
            using allocator::deallocate_policy;
            using allocator::copy_policy;
            using allocator::compare_policy;
            //expose compressor functions
            using compressor::compress_policy;
            using compressor::uncompress_policy;

            public:
            typedef std::size_t size_type;
            typedef allocator allocator_type;
            // seems like T was only typedef'd for use in other typedefs, interesting
            typedef T value_type;
            typedef value_type *pointer;
            typedef pointer iterator;
            typedef const value_type *const_pointer;
            typedef value_type &reference;
            typedef const value_type &const_reference;

            // m = 1 one instance only !
            // constructor given dimensions
            block(size_type n = 1, size_type m = 1) : rows_(m) {
                dim0_ = n;                                      // dim0 not necessary = num_cols due to the resize
                cols_ = resize_helper<allocator>(n, sizeof(T)); // some policy will resize the col, needs for 2D !
                current_size = sizeof(T) * cols_ * rows_;
                data_ = (pointer)allocate_policy(current_size);
            }
            //constructor given rval to another block
            block(block &&other) : rows_(other.rows_), cols_(other.cols_), dim0_(other.dim0_), data_(other.data_) {
                // std::move is not needed on basic type
                other.rows_ = 0;
                other.cols_ = 0;
                other.dim0_ = 0;
                current_size = 0;
                other.data_ = nullptr;
            }

            block(const block &other) {
                // std::move is not needed on basic type
                rows_ = other.rows_;
                cols_ = other.cols_;
                dim0_ = other.dim0_;
                current_size = sizeof(T) * cols_ * rows_;
                data_ = (pointer)allocate_policy(current_size);
                copy_policy(data_, other.data_, current_size);
            }


            //copy = operator
            block &operator=(block &&rhs) {
                rows_ = rhs.rows_;
                cols_ = rhs.cols_;
                dim0_ = rhs.dim0_;
                data_ = rhs.data_;
                current_size = rhs.current_size;
                rhs.rows_ = 0;
                rhs.cols_ = 0;
                rhs.dim0_ = 0;
                rhs.data_ = nullptr;

                // avoid destruction here



                return *this;
            }

            ~block() {
                if (data_ != nullptr) // due to move constructor
                    deallocate_policy(data_);
            }


            void resize(size_type n = 1, size_type m = 1) {
                // essentially just getting which is smaller the resize or the current
                if (n != dim0() || m != dim1()) {
                    const auto copy_cols = n < dim0() ? n : dim0();
                    const auto copy_rows = m < dim1() ? m : dim1();

                    // new block will be destroy at the end of the scopr
                    // and data are echange due to the std::move and consequent std::swap
                    block new_block(n, m); // function probably lives in allocator

                    for (size_type r(0); r < copy_rows; ++r)
                        copy_policy(&new_block[r], &(*this)[r], sizeof(T) * copy_cols);

                    *this = std::move(new_block);
                }
            }

            // stl compatibility
            iterator begin() { return data_; }
            iterator end() { return data_ + dim0_ * rows_; }

            //specialized iterator nestedclass
            class iter : public std::iterator<std::bidirectional_iterator_tag,value_type> {
                block<value_type, allocator_type> blk;
                size_type col_ind,row_mult;
                public:
                iter (const block<value_type,allocator_type>&  blk_in, size_type col,size_type row) 
                    : blk {blk_in} ,col_ind {col}, row_mult{row} {}

                iter operator ++ (int) {// postfix needed for forward iterator
                    iter temp(blk,col_ind,row_mult);
                    row_mult++;
                    return temp;
                }

                iter& operator -- () {
                    row_mult--;
                    return *this;
                }

                iter operator--  (int) {
                    iter temp(blk,col_ind,row_mult);
                    row_mult--;
                    return temp;
                }

                iter& operator ++ () {
                    row_mult++;
                    return *this;
                }

                value_type& operator* () {
                    return *(blk.data() + row_mult*blk.dim0() +col_ind);
                }


                bool operator == (const iter &rhs) {
                    return row_mult == rhs.row_mult;
                }
                bool operator != (const iter &rhs) {
                    return row_mult != rhs.row_mult;
                }

                void operator = (const iter &rhs) {
                    row_mult = rhs.row_mult;
                }
                // last things needed by a bidirectional iterator
                iter & operator += (const iter & rhs) {
                    row_mult += rhs.row_mult;
                    return *this;
                }

                template<typename num_t>
                iter & operator += (const num_t & rhs) {
                    row_mult += (size_type) rhs;
                    return *this;
                }
                template<typename num_t>
                iter & operator -= (const num_t & rhs) {
                    row_mult -= (size_type) rhs;
                    return *this;
                }

                // does this need to be greater than 0 at all times?
                size_type operator - (const iter &rhs) {
                    return row_mult - rhs.row_mult;
                }

                size_type operator +( const iter & rhs) {
                    return row_mult - rhs.row_mult;
                }

                value_type operator [] (const int & ind) {
                    // make use of the existing + operator and *
                    return *(this + ind);
                }

                // commutative + operators for the iterator
                template<typename num_t>
                iter operator + (const num_t & lhs) {
                    iter ret_iter{*this};
                    ret_iter += lhs;
                    return ret_iter;
                }

                template<typename num_t>
                iter operator - ( const num_t & rhs) {
                    iter ret_iter{*this};
                    ret_iter -= rhs;
                    return ret_iter;
                }

                //total ordering operators

                bool operator > (const iter & rhs) const{
                    value_type rhs_val = *rhs;
                    value_type this_val = **this;
                    return this_val > rhs_val;
                }
                bool operator < (const iter & rhs) const{
                    value_type rhs_val = *rhs;
                    value_type this_val = **this;
                    return this_val < rhs_val;
                }
                bool operator <= (const iter & rhs) const{
                    value_type rhs_val = *rhs;
                    value_type this_val = **this;
                    return this_val <= rhs_val;
                }
                bool operator >= (const iter & rhs) const{
                    value_type rhs_val = *rhs;
                    value_type this_val = **this;
                    return this_val >= rhs_val;
                }



            };

            void col_iter () {
                iter start(*this,0,0);
                iter end(*this,0,rows_);
                // testing bidirectionality with move_backward 
                iter other_start(*this,1,0);
                iter other_end(*this,1,rows_);
                //sort the first two columns
                std::sort(start,end);
                std::sort(other_start,other_end);
            }






            //difference between memory_allocated and size is that allocated relies on construction size, where size depends on compression
            size_type memory_allocated() const { return sizeof(T) * cols_ * rows_; }

            bool is_compressed() {return compression_state;}

            size_type get_current_size() {return current_size;}

            const_pointer data() const { return data_; };

            pointer data() { return data_; };

            size_type dim0() const { return dim0_; }
            size_type dim1() const { return rows_; }// supposed to be dim1?
            // alias
            size_type num_cols() const { return cols_; }
            size_type num_rows() const { return rows_; }

            reference operator[](size_type i) { return (*this)(0, i); }// what function is getting called here?

            const_reference operator[](size_type i) const { return (*this)(0, i); }

            reference operator()(size_type i, size_type j = 0) {
                // determines if i j are legal
                assert(i <= cols_);
                assert(j <= rows_);
                return data_[i + j * cols_];
            }

            const_reference operator()(size_type i, size_type j = 0) const {
                assert(i <= cols_);
                assert(j <= rows_);
                return data_[i + j * cols_];
            }

            //when given the std::cout for the standard operator we add to its stream
            std::ostream & print(std::ostream & os) const {
                for (int i = 0; i < dim1(); ++i) { // raw first
                    for (int j = 0; j < dim0(); ++j) {
                        //precision defines accuracy for testing comparison
                        os << std::setprecision(15) << (*this)(j, i);
                        os << (j != dim0()-1 ? " " : "");// prevent trailing whitespace in output
                    }
                    os << (i != dim1() -1? "\n": "") ;
                }
                return os;
            }

            bool operator == (const block & other) {
                //check size matches first
                return compare_policy(this->data(),other.data(),current_size);
            }

            bool operator != (const block & other) {
                //should just be the opposite of the existing compare_policy
                return ! compare_policy(this->data(),other.data(),current_size);
            }




            // this is the tool for adding entries to our block
            void read(std::istream & file_in)
            {
                //get the dimensions of the block from the first two values in the data, (col,row)
                std::string line;
                int row,col;
                file_in >> col;
                file_in >> std::ws;
                // this comes up in cases where an empty string is provided,
                if(file_in.get() != ',') {
                    throw 0;// I think it just means if there was something inbetween that wasn't a comma throw 0 error
                }
                //and now repeat for the row
                file_in >> row;
                file_in >> std::ws;
                //make block match type value_type of calling block
                block<value_type,allocator_type> b(col,row);
                //take full line
                row = 0;// start at first row for entering values
                while(std::getline(file_in,line) && row < b.num_rows()) {
                    //reset the column count to enter data for first column
                    col = 0;
                    //split on commas
                    std::stringstream comma_splitter(line);
                    std::string data_cell;
                    //read from stream comma_splitter, split on comma, and enter into the data_cell string
                    while(std::getline(comma_splitter,data_cell,',') && col < b.dim0()) {
                        // using the block element indexing
                        std::stringstream(data_cell) >> std::dec >> b(col++,row);
                    }
                    row++;
                }
                // now we have to swap the data in this block with the calling object block data
                std::swap(*this,b);
            }

            // block access to compression functions included via policy
            // make into data ref and size as arguments
            //
            void compress() {
                compress_policy(&data_,&current_size);
                compression_state = true;
            }
            void uncompress() {
                uncompress_policy(&data_,&current_size,this->memory_allocated());
                compression_state = false;
            } 

            private:
            size_type rows_;
            size_type cols_;
            size_type dim0_;
            pointer data_;
            //compression members
            size_type current_size;
            bool compression_state = false;
        };

    template <class T, class A>
        std::ostream &operator<<(std::ostream &out, block<T, A> &b) {
            b.print(out);
            return out;
        }

    //follow Tim's pattern with the outbound ostream above
    template <class T, class A>
        std::istream & operator >> (std::istream & in, block<T,A> &b ) {
            b.read(in);// create contents of block based on data in inputstream
            return in;
        }



} // namespace neuromapp


#endif
