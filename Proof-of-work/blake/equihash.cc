void Equihash::InitializeMemory()
{
    uint32_t  tuple_n = ((uint32_t)1) << (n / (k + 1));
    Tuple default_tuple(k); // k blocks to store (one left for index)
    std::vector<Tuple> def_tuples(LIST_LENGTH, default_tuple);
    tupleList = std::vector<std::vector<Tuple>>(tuple_n, def_tuples);
    filledList= std::vector<unsigned>(tuple_n, 0);
    solutions.resize(0);
    forks.resize(0);
}

void Equihash::PrintTuples(FILE* fp) {
    unsigned count = 0;
    for (unsigned i = 0; i < tupleList.size(); ++i) {
        for (unsigned m = 0; m < filledList[i]; ++m) {
            fprintf(fp, "[%d][%d]:", i,m);
            for (unsigned j = 0; j < tupleList[i][m].blocks.size(); ++j)
                fprintf(fp, " %x ", tupleList[i][m].blocks[j]);
            fprintf(fp, " || %x", tupleList[i][m].reference);
            fprintf(fp, " |||| ");
        }
        count += filledList[i];
        fprintf(fp, "\n");
    }
    fprintf(fp, "TOTAL: %d elements printed", count);
}

void Equihash::FillMemory(uint32_t length) //works for k<=7
{
    uint32_t input[SEED_LENGTH + 2];
    for (unsigned i = 0; i < SEED_LENGTH; ++i)
        input[i] = seed[i];
    input[SEED_LENGTH] = nonce;
    input[SEED_LENGTH + 1] = 0;
    uint32_t buf[MAX_N / 4];
    for (unsigned i = 0; i < length; ++i, ++input[SEED_LENGTH + 1]) {
        blake2b((uint8_t*)buf, &input, NULL, sizeof(buf), sizeof(input), 0);
        uint32_t index = buf[0] >> (32 - n / (k + 1));
        unsigned count = filledList[index];
        if (count < LIST_LENGTH) {
            for (unsigned j = 1; j < (k + 1); ++j) {
                //select j-th block of n/(k+1) bits
                tupleList[index][count].blocks[j - 1] = buf[j] >> (32 - n / (k + 1));
            }
            tupleList[index][count].reference = i;
            filledList[index]++;
        }
    }
}
