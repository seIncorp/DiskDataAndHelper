/*
 * _d_: left to right OR right to left
 * _v_: number OR char
 * _t_: int OR char OR long long OR ...
 * */
#define SHOW_IN_BITS(_d_, _v_, _t_) ({ \
     \
    if(_d_ == 1) \
    { \
        for(int _ii_ = (sizeof(_t_)*8)-1; _ii_ >=0 ; _ii_--){ \
            o << (( (_v_) >> _ii_) & 1); } \
    } \
     \
    if(_d_ == 0) \
    { \
        for(int _ii_ = 0; _ii_ < (sizeof(_t_)*8) ; _ii_++){ \
            o << (( (_v_) >> _ii_) & 1); } \
    } \
}) 

 /*
  * _v_: value
  * _p_: position
  * return: new value
  * */
#define SET_BIT_INT(_v_, _p_) ({ \
    int _res_ = 0; \
    _res_ = ( (_v_) | (1 << _p_) ); \
    _res_; \
})

  /*
   * _v_: value
   * _p_: position
   * return: new value
   * */
#define CLEAR_BIT_INT(_v_, _p_) ({ \
    int _res_ = 0; \
    _res_ = ( (_v_) & ( ~(1 << _p_) ) ); \
    _res_; \
})

   /*
    * _v_: value
    * return: 0 - FALSE OR 1 - TRUE
    * */
#define IS_EVEN(_v_) ({ \
    bool _xcy_ =  (((_v_) & 1 ) == 0); \
    _xcy_; \
})

    /*
     * _v_: value
     * _t_: int OR long OR ...
     * return: 0 - FALSE OR 1 - TRUE
     * */
#define IS_POSITIVE_NUMBER(_v_,_t_) ({ \
    bool _xcy_ = (((_v_) >> ((sizeof(_t_)*8)-1)) & 1) == 0; \
    _xcy_; \
})

     /*
      * _v_: value
      * return: new value
      * */
#define MULTIPLY_BY_TWO(_v_) ({ \
    int _res_ = (_v_) << 1; \
    _res_; \
})

      /*
       * _v_: value
       * return: new value
       *
       * WARNING: ODD numbers will be showing without decimal numbers (like as intiger)
       * */
#define DIVIDE_BY_TWO(_v_) ({ \
    int _res_ = (_v_) >> 1; \
    _res_; \
})

#define SWITCH_SIGN(_v_) ({ \
    int _res_ = ~(_v_) + 1; \
    _res_; \
})

       /*
        * _n1_: number 1
        * _n2_: number 2
        * return: new value
        * */
#define MULTIPLAY_TWO_UNSIGNET_NUMBERS(_n1_,_n2_) ({ \
    int _res_ = 0; \
    int _temp1_ = (_n1_); \
    int _temp2_ = (_n2_); \
    int _posBit_ = 0; \
    while (_temp2_ != 0) { \
        if (_temp2_ & 1) \
        { \
            _res_ += (_temp1_ << _posBit_); \
        } \
         \
        _posBit_ += 1; \
        _temp2_ >>= 1; \
    } \
    _res_; \
})

        /*
         * _v_: value
         * return: counted of 1's
         * */
#define COUNT_SET_BITS(_v_) ({ \
    int _res_ = 0; \
    int _temp1_ = (_v_); \
    while (_temp1_ != 0) { \
        _res_ += (_temp1_ & 1); \
        _temp1_ >>= 1; \
    } \
    _res_; \
})

         /*
          * _n1_: number 1
          * _n2_: number 2
          * return: counted of different bits
          * */
#define COUNT_BITS_DIFF(_n1_,_n2_) ({ \
    int _res2_ = COUNT_SET_BITS(_n1_ ^ _n2_); \
    _res2_; \
})

          /*
           * _n1_: number 1
           * return: bits length of the number
           * */
#define BITS_LENGTH(_n1_) ({ \
    int _res_ = 0; \
    while( (1 << _res_) <= (_n1_) ) \
    { \
        _res_ += 1; \
    } \
    _res_; \
})