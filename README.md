# SmallDB
A very simple database made with pure C++

## Types

* Double
* Long
* String (with max string length)
* Datetime (no timezone)

## Operations

* insertRow()
* find()
* readDouble(), writeDouble()
* readLong(), writeLong()
* readString(), writeString()
* readDatetime(), writeDatetime()
* delete()

## Architecture

The general arch is expressed in [architecture.txt](https://github.com/EmmanuelMess/SmallDB/blob/0436bdd5642b5b6e8bb3bbe5a8a2452b1bdb5cec/architecture.txt).

### File layout
        |----------------|--------|-------|-------|-------|-------|-------|-   -|
        | number of rows | header | row 0 | row 1 | row 2 | row 3 | row 4 | ... |
        |----------------|--------|-------|-------|-------|-------|-------|-   -|
    
      Header:
        |---------------|------|-------------|------|-------------|-   -|------------|
        | header length | name | type letter | name | type letter | ... | row length |
        |---------------|------|-------------|------|-------------|-   -|------------|
    
        * name is std::string_view::c_str() with unicode support
    
        type letter:
          |-----|-----------------|     |-----|     |-----|     |-----|
          | 'S' | max size column | or  | 'D' | or  | 'L' | or  | 'T' |
          |-----|-----------------|     |-----|     |-----|     |-----|
    
          * type letter is the id letter for Datatype
          * max size column is the max size of the column in bytes, it is only used for String
    
      Row:
        |----------|----------|----------|-   -|
        | column 0 | column 1 | column 2 | ... |
        |----------|----------|----------|-   -|
    
        * row has predefined length according to each type for Double, Long and Datetime
        * if there is a string_view in the row, there will be length information in header
    
      Serialization for Datetime:
        |------|-------|-----|------|--------|--------|
        | year | month | day | hour | minute | second |
        |------|-------|-----|------|--------|--------|
     
        year, month, day, hour, minute, second are saved as uint32_t
    
      Serialization for string:
        |-          -|-       -|
        | characters | padding |
        |-          -|-       -|
    
        * pad with 0 until assigned string size is filled (for row skipping conveniences)
     
      * All lengths are uint32_t

----
<a class="imgpatreon" href="https://www.patreon.com/emmanuelmess" target="_blank">
<img alt="Become a patreon" src="https://user-images.githubusercontent.com/10991116/56376378-07065400-61de-11e9-9583-8ff2148aa41c.png" width=150px></a>
