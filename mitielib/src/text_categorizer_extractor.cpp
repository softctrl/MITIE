// Copyright (C) 2014 Massachusetts Institute of Technology, Lincoln Laboratory
// License: Boost Software License   See LICENSE.txt for the full license.
// Authors: Davis E. King (davis@dlib.net)

#include <mitie/text_categorizer_extractor.h>
#include <mitie/text_feature_extraction.h>

using namespace dlib;

namespace mitie
{

// ----------------------------------------------------------------------------------------

    text_categorizer_extractor::
    text_categorizer_extractor(
        const std::vector<std::string>& tag_name_strings_,
        const total_word_feature_extractor& fe_,
        const dlib::multiclass_linear_decision_function<dlib::sparse_linear_kernel<text_sample_type>,unsigned long>& df_
    ) : tag_name_strings(tag_name_strings_), fe(fe_), df(df_)
    {
        // make sure the requirements are not violated.
        DLIB_CASSERT(df.number_of_classes() >= tag_name_strings.size(),"invalid inputs");

        std::set<unsigned long> df_tags(df.get_labels().begin(), df.get_labels().end());
        for (unsigned long i = 0; i < tag_name_strings.size(); ++i)
        {
            DLIB_CASSERT(df_tags.count(i) == 1, "The classifier must be capable of predicting each possible tag as output.");
        }

        compute_fingerprint();
    }

    text_categorizer_extractor::
    text_categorizer_extractor(const std::string& pureModelName,
                           const std::string& extractorName
    ) {
        std::string classname;
        dlib::deserialize(pureModelName) >> classname;
        if (classname != "mitie::text_categorizer_extractor_pure_model")
            throw dlib::error(
                    "This file does not contain a mitie::text_categorizer_extractor_pure_model. Contained: " + classname);

        dlib::deserialize(pureModelName) >> classname >> df >> tag_name_strings;

        dlib::deserialize(extractorName) >> classname;
        if (classname != "mitie::total_word_feature_extractor")
            throw dlib::error(
                    "This file does not contain a mitie::total_word_feature_extractor. Contained: " + classname);

        dlib::deserialize(extractorName) >> classname >> fe;
    }
// ----------------------------------------------------------------------------------------

    void text_categorizer_extractor::
    predict (
        const std::vector<std::string>& sentence,
        unsigned long& text_tag,
        double& text_score
    ) const {
        const std::vector<matrix<float, 0, 1> > &sent = sentence_to_feats(fe, sentence);

        // now label the document
        const std::pair<unsigned long, double> temp = df.predict(extract_text_features(sentence, sent));
        text_tag = temp.first;
        text_score = temp.second;
    }

// ----------------------------------------------------------------------------------------

    void text_categorizer_extractor::
    operator() (
        const std::vector<std::string>& sentence,
        unsigned long& text_tag
    ) const
    {
        const std::vector<matrix<float,0,1> >& sent = sentence_to_feats(fe, sentence);

        // now label the document
        const std::pair<unsigned long, double> temp = df.predict(extract_text_features(sentence, sent));
        text_tag = temp.first;
    }

// ----------------------------------------------------------------------------------------

}

