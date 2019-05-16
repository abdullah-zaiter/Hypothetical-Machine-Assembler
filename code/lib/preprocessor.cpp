#include <preprocessor.h>


PreProcessor::PreProcessor(std::fstream *source)
{
    this->source_code_file = source;
}

PreProcessor::~PreProcessor(){}

void PreProcessor::printSourceFile(void)
{
    PreProcessor::printFile(this->source_code_file);
}

void PreProcessor::printFile(std::fstream *source)
{
    std::string line;
    while (*(source) >> line) {
        std::cout << line << std::endl;
    }
    source->clear();
    source->seekg( 0, source->beg);
    source->seekp( 0, source->beg);
}

std::vector<uint16_t> PreProcessor::findCharInStringIndexes(std::string sample, char character_to_find)
{
    std::vector<uint16_t> character_indexes;
    for(uint16_t i =0; i < sample.size(); i++)
    {
        if(sample[i] == character_to_find)
        {  
            character_indexes.push_back(i);
        }
    }
    return character_indexes;
}

std::vector<std::string> PreProcessor::splitString(std::string word)
{
    std::vector<std::string> splitted_words;
    std::vector<char> special_characters{SPECIAL_CHARACTERS};
    std::vector<uint16_t> splitting_indexes;
    splitting_indexes.push_back(0);
    for(uint16_t i = 0 ; i<=special_characters.size(); i++)
    {
        auto aux = findCharInStringIndexes(word,special_characters[i]);
        splitting_indexes.insert(splitting_indexes.end(),aux.begin(),aux.end());
    }
    std::sort(splitting_indexes.begin(), splitting_indexes.end());
    uint16_t last_index = 0;
    for (uint16_t i = 1; i < splitting_indexes.size(); i++)
    {
        auto aux = word.substr(last_index,splitting_indexes[i]-last_index);
        //adding the word with no special characters
        if(!(aux.empty()))
        {
            splitted_words.push_back(aux);
        }
        ///adding the special character found IF NOT EMPTY
        if(!(std::string(1,word[splitting_indexes[i]]).empty()))
        {
            splitted_words.push_back(std::string(1,word[splitting_indexes[i]]));
        }
        last_index = splitting_indexes[i]+1;
    }
    //this condition treats the cases where a word starts with a special character, pushes the rest of the string as a word to the splitted words
    if((splitting_indexes.back()) != (word.length()-1) && splitting_indexes.size() > 1)
    {
        auto aux = word.substr(splitting_indexes.back()+1,word.length()-1-splitting_indexes.back());
        splitted_words.push_back(aux);
    }
    //this condition treats the case where theres no any special characters in the string
    if(splitting_indexes.size() == 1 && splitting_indexes[0] == 0 )
    {
        splitted_words.push_back(word);
    }

    return splitted_words;
}

TupleList PreProcessor::removeUselessInfos(void)
{ 
    std::string text_line, word;
    std::vector <std::string> sentence;
    std::tuple<uint16_t,std::vector <std::string>> current_tuple;
    TupleList tuple_file;
    uint16_t line_cont = 0;
    while (std::getline(*(this->source_code_file), text_line))
    {
        std::istringstream text_stream(text_line);
        while (text_stream >> word)
        {
            std::transform(word.begin(), word.end(),word.begin(), ::toupper);
            auto aux = splitString(word);
            sentence.insert(sentence.end(),aux.begin(),aux.end());
        }
        if(!(text_line.empty()))
        {
            current_tuple = make_tuple(line_cont,sentence); 
            tuple_file.push_back(current_tuple);
            sentence.clear();
        }
        line_cont++;
    }
    return tuple_file;
}

void PreProcessor::printTupleListFile()
{
    std::cout << "_____________________"<<std::endl;
    
    for(size_t i = 0; i != this->file_being_processed.size(); i++ )
    {
        std::cout << std::setfill('0') << std::setw(3) << std::get<0>(this->file_being_processed[i])+1 << " ";
        for(size_t j = 0; j != std::get<1>(this->file_being_processed[i]).size(); j++ )
            std::cout << std::get<1>(this->file_being_processed[i])[j] << " ";
        std::cout << std::endl;
    } 
    std::cout << "_____________________"<<std::endl;
}


void PreProcessor::printMDT()
{
    std::cout << "_____________________\nMDT"<<std::endl;
    
    for(size_t i = 0; i != this->mdt.size(); i++ )
    {
        std::cout << std::setfill('0') << std::setw(3) << std::get<0>(this->mdt[i])+1 << " ";
        for(size_t j = 0; j != std::get<1>(this->mdt[i]).size(); j++ )
                std::cout << std::get<1>(this->mdt[i])[j] << " ";
        std::cout << std::endl;
    } 
    std::cout << "MDT\n_____________________"<<std::endl;
}


void PreProcessor::printMNT()
{
    std::cout << "___________________MNT\n"<<std::endl;
    
    for(size_t i = 0; i != this->mnt.size(); i++ )
        std::cout<<std::get<0>(this->mnt[i])<<" "<<(int)std::get<1>(this->mnt[i])<<" "<<(int)std::get<2>(this->mnt[i])<<std::endl; 
    std::cout << "MNT\n___________________"<<std::endl;
    
}

void PreProcessor::printTupleTable(Table tuple_table)
{
    std::cout << "_____________________"<<std::endl;
    for(size_t i = 0; i != tuple_table.size(); i++ )
    {
        std::cout << std::get<0>(tuple_table[i]) << " = " << std::get<1>(tuple_table[i]) << std::endl;
    } 
    std::cout << "_____________________"<<std::endl;

}
Table PreProcessor::parseDirectiveEQU(void)
{
    Table equs;
    std::tuple<std::string,std::string> current_tuple;
    for(size_t i = 0; i != this->file_being_processed.size(); i++ )
    {
        if(std::get<1>(this->file_being_processed[i]).size()>=3 && std::get<1>(this->file_being_processed[i])[2] == "EQU")
        {
            current_tuple = make_tuple(std::get<1>(this->file_being_processed[i])[0],std::get<1>(this->file_being_processed[i])[3]);
            if (std::get<1>(this->file_being_processed[i])[3] != "1" && std::get<1>(this->file_being_processed[i])[3] != "0")
            {
                std::cout<<"Warning: EQU of line : "<<std::get<0>(this->file_being_processed[i]) + 1<<" in original source code, is not 0 nor 1."<<std::endl;
            }
            equs.push_back(current_tuple);
            this->file_being_processed.erase(this->file_being_processed.begin()+(i--));
        }
    }
    return equs; 
}

std::string PreProcessor::findInTable(Table tbl, std::string id)
{
    for(size_t i = 0; i != tbl.size() ; i++)
    {
        if(std::get<0>(tbl[i]) == id)
        {
            return std::get<1>(tbl[i]);
        } 
    }
    return "NULL";
}

void PreProcessor::processEQUs(void)
{
    for(size_t i = 0; i != this->file_being_processed.size(); i++ )
    {
        for(size_t j = 0; j != std::get<1>(this->file_being_processed[i]).size(); j++ )
        {
            auto aux = findInTable(this->table_EQU,std::get<1>(this->file_being_processed[i])[j]);
            if(aux != "NULL")
            {
                std::get<1>(this->file_being_processed[i])[j] = aux;
            }
        }   
    }
}

void PreProcessor::processIFs(void)
{
    for(size_t i = 0; i != this->file_being_processed.size(); i++ )
    {
        if(std::get<1>(this->file_being_processed[i])[0] == "IF")
        {
            if(std::get<1>(this->file_being_processed[i])[1] == "1")
            {
                this->file_being_processed.erase(this->file_being_processed.begin()+(i--));
            } else if (std::get<1>(this->file_being_processed[i])[1] == "0")
            {
                this->file_being_processed.erase(this->file_being_processed.begin()+(i--));
                this->file_being_processed.erase(this->file_being_processed.begin()+(i--)+1);
            }else
            {
                std::cout<<"Error: IF of line : "<<std::get<0>(this->file_being_processed[i]) + 1<<" in original source code, condition is not 0 nor 1 (or condition is not defined before) : ";
                std::cout << std::get<1>(this->file_being_processed[i])[1] << std::endl;
            }
        }
    }
}

void PreProcessor::removeComments(void)
{
    bool found = false;
    for(size_t i = 0; i != this->file_being_processed.size(); i++ )
    {                   
        found = false;
        for(size_t j = 0; j != std::get<1>(this->file_being_processed[i]).size() && !found ; j++ )
        {          
            if(std::get<1>(this->file_being_processed[i])[j] == ";")
            {
                if (j == 0)
                {
                    this->file_being_processed.erase(this->file_being_processed.begin()+(i--));
                    break;
                }
                std::get<1>(this->file_being_processed[i]).erase(std::get<1>(this->file_being_processed[i]).begin()+j, std::get<1>(this->file_being_processed[i]).end()-1);
                std::get<1>(this->file_being_processed[i]).pop_back();
                found = true;
            }
        }   
    }
}

bool PreProcessor::insertInMacroNameTable(size_t i, size_t position_macro) 
{
    size_t number_of_tokens = ELEMENT_FBP(1, i).size();
    if ((number_of_tokens >= MIN_MACRO) && (number_of_tokens <= MAX_MACRO))
    {
        //TODO: if (1° elemento não é uma função)
        if ((position_macro == 2) && (ELEMENT_FBP(1, i)[1] == ":"))
        {
            std::string label_macro = ELEMENT_FBP(1, i)[0];
            auto it = std::find_if(this->mnt.begin(), this->mnt.end(), [&](const std::tuple<std::string,uint8_t,uint8_t>& e) {return std::get<0>(e) == label_macro;});
            if (it == this->mnt.end())
            {
                uint8_t n_commas = 0;
                std::vector<std::string> arguments;
                if (MACRO_HAS_PARAMETERS(number_of_tokens))
                {
                    bool should_be_a_comma = 0;
                    for (size_t j = 3; j < number_of_tokens; j++)
                    {
                        auto repeated_parameter = std::find(arguments.begin(), arguments.end(), ELEMENT_FBP(1,i)[j]);
                        if (repeated_parameter == arguments.end())
                        {
                            if (!should_be_a_comma && ELEMENT_FBP(1,i)[j][0] == '&')
                                arguments.push_back(ELEMENT_FBP(1,i)[j]);
                            else if (should_be_a_comma && ELEMENT_FBP(1,i)[j][0] == ',')
                                n_commas++;   
                            else
                                std::cout << std::setfill('0') << std::setw(3) << ELEMENT_FBP(0, i)+1 << ": Error: Invalid argument" << std::endl;
                            should_be_a_comma = !should_be_a_comma;
                        }
                        else
                        {
                            std::cout << std::setfill('0') << std::setw(3) << ELEMENT_FBP(0, i)+1 << ": Error: Repeated parameter" << std::endl;
                            break;
                        }                    
                    }
                }
                if (THERE_ARE_MORE_COMMAS(number_of_tokens))
                    std::cout << std::setfill('0') << std::setw(3) << ELEMENT_FBP(0, i)+1 << ": Warning: There are more commas than expected" << std::endl;
                
                std::tuple<std::string, uint8_t,uint8_t> element_mnt(ELEMENT_FBP(1, i)[0], (number_of_tokens-MIN_MACRO-(int)n_commas), this->mdt.size());
                this->mnt.push_back(element_mnt);
                this->file_being_processed.erase(this->file_being_processed.begin()+i);
                PreProcessor::insertInMacroDefinitionTable(i, arguments);
                return 1;
            }
            else
                std::cout << std::setfill('0') << std::setw(3) << ELEMENT_FBP(0, i)+1 << ": Error: Repeated label" << std::endl;
        }
        else
            std::cout << std::setfill('0') << std::setw(3) << ELEMENT_FBP(0, i)+1 << ": Error: Invalid directive" << std::endl;
    }
    else if (number_of_tokens > MAX_MACRO)
        std::cout << std::setfill('0') << std::setw(3) << ELEMENT_FBP(0, i)+1 << ": Error: Invalid argument" << std::endl;
    else
        std::cout << std::setfill('0') << std::setw(3) << ELEMENT_FBP(0, i)+1 << ": Error: Missing macro label" << std::endl;    
    return 0;
}

void PreProcessor::insertInMacroDefinitionTable(size_t pos, std::vector<std::string> arg)
{
    while (ELEMENT_FBP(1, pos)[0] != "END")
    {
        this->mdt.push_back(this->file_being_processed[pos]);  
        this->file_being_processed.erase(this->file_being_processed.begin()+pos);
        for (size_t i = 0; i < arg.size(); i++)
        {
            auto parameter = std::find(std::get<1>(this->mdt.back()).begin() , std::get<1>(this->mdt.back()).end() , arg[i] );
            while (parameter != std::get<1>(this->mdt.back()).end())
            {
                *parameter = "#" + std::to_string(i);
                parameter = std::find(std::get<1>(this->mdt.back()).begin() , std::get<1>(this->mdt.back()).end() , arg[i] );
            }
        }
    }
    this->mdt.push_back(this->file_being_processed[pos]);  
    this->file_being_processed.erase(this->file_being_processed.begin()+pos);
}

bool PreProcessor::swapLinesMacro(size_t line, size_t position_macro)
{
    auto n_arguments = std::get<1>(this->mnt[position_macro]);
    auto n_commas = 0;
    if (n_arguments > 0)
        n_commas = n_arguments - 1;
    if (  (std::get<1>(this->file_being_processed[line]).size() - 1 - n_commas) != (n_arguments)  )
    {
        std::cout << std::setfill('0') << std::setw(3) << ELEMENT_FBP(0, line)+1 << ": Error: Number of arguments invalid" << std::endl;
        return 0;
    }
        
    auto macro_body_start = std::get<2>(this->mnt[position_macro]);
    
    std::vector<std::string> arg, name_arg;
    for (size_t j = 0; j < n_arguments; j++)
    {
        arg.push_back(  ("#" + std::to_string(j))  );
        name_arg.push_back(  ELEMENT_FBP(1, line)[( (j<<1) + 1 )]  );
    }
    this->file_being_processed.erase(this->file_being_processed.begin() + line);
    for (size_t i = 0; std::get<1>(this->mdt[macro_body_start+i])[0] != "END" ; i++)
    {   
        this->file_being_processed.insert(this->file_being_processed.begin()+line+i, this->mdt[macro_body_start+i]);    
        for (size_t k = 0; k < n_arguments; k++)
        {
            auto find_parameter=std::find(ELEMENT_FBP(1, (line+i) ).begin(), ELEMENT_FBP(1, (line+i) ).end(), arg[k]);
            while (find_parameter != ELEMENT_FBP(1, (line+i) ).end())
            {   
                *find_parameter = name_arg[k];
                find_parameter =std::find(ELEMENT_FBP(1, (line+i) ).begin(), ELEMENT_FBP(1, (line+i) ).end(), arg[k]);
            }   
        }
    }
    return 1;
}

void PreProcessor::processMacros(void)
{
    for(size_t i = 0; i != this->file_being_processed.size(); i++ )
    {
        auto is_it_a_macro = std::find(ELEMENT_FBP(1, i).begin(), ELEMENT_FBP(1, i).end(), "MACRO");
        if (is_it_a_macro != ELEMENT_FBP(1, i).end())
            if (PreProcessor::insertInMacroNameTable(i, (is_it_a_macro - ELEMENT_FBP(1, i).begin())))
                if (i!=0)
                    i--;
        
        std::string first_token = ELEMENT_FBP(1,i)[0];
        auto is_this_macro_label = std::find_if(this->mnt.begin(), this->mnt.end(), [&](const std::tuple<std::string,uint8_t,uint8_t>& e) {return std::get<0>(e) == first_token;});
        if (is_this_macro_label != this->mnt.end())
            if (PreProcessor::swapLinesMacro(i, (is_this_macro_label - this->mnt.begin())))
                i--;
    } 
}

TupleList PreProcessor::preProcess(void)
{
    this->file_being_processed = removeUselessInfos(); 
    removeComments();
    this->table_EQU = parseDirectiveEQU();
    //printTupleTable(this->table_EQU);
    processEQUs();
    //printTupleListFile();
    processIFs();
    printTupleListFile();
    processMacros();
    //printMNT();
    //printMDT();
    printTupleListFile();
    return  this->file_being_processed;
}