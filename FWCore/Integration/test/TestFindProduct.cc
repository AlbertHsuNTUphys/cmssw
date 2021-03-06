
// This test module will look for IntProducts in Events.
// The number of IntProducts and their InputTags (label,
// instance, process) must be configured.

// One can also configure an expected value for the sum of
// all the values in the IntProducts that are found.  Note
// that an IntProduct is just a test product that simply
// contains a single integer.

// If the products are not found, then an exception is thrown.
// If the sum does not match there is an error message and
// an abort.

#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/TestObjects/interface/ToyProducts.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/getProducerParameterSet.h"
#include "FWCore/Framework/interface/GetterOfProducts.h"
#include "FWCore/Framework/interface/LuminosityBlock.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/ProcessBlock.h"
#include "FWCore/Framework/interface/ProcessMatch.h"
#include "FWCore/Framework/interface/Run.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/BranchType.h"
#include "FWCore/Utilities/interface/Exception.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Utilities/interface/EDGetToken.h"
#include <iostream>
#include <vector>

namespace edmtest {

  class TestFindProduct
      : public edm::one::EDAnalyzer<edm::one::WatchRuns, edm::one::WatchLuminosityBlocks, edm::WatchProcessBlock> {
  public:
    explicit TestFindProduct(edm::ParameterSet const& pset);
    virtual ~TestFindProduct();

    void analyze(edm::Event const& e, edm::EventSetup const& es) override;
    void beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;
    void endLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) override;
    void beginRun(edm::Run const&, edm::EventSetup const&) override;
    void endRun(edm::Run const&, edm::EventSetup const&) override;
    void beginProcessBlock(edm::ProcessBlock const&) override;
    void endProcessBlock(edm::ProcessBlock const&) override;
    void endJob() override;

  private:
    std::vector<edm::InputTag> inputTags_;
    int expectedSum_;
    int sum_;
    std::vector<edm::InputTag> inputTagsNotFound_;
    bool getByTokenFirst_;
    std::vector<edm::InputTag> inputTagsView_;
    bool runProducerParameterCheck_;
    bool testGetterOfProducts_;

    std::vector<edm::InputTag> inputTagsUInt64_;
    std::vector<edm::InputTag> inputTagsEndLumi_;
    std::vector<edm::InputTag> inputTagsEndRun_;
    std::vector<edm::InputTag> inputTagsBeginProcessBlock_;
    std::vector<edm::InputTag> inputTagsEndProcessBlock_;
    std::vector<edm::InputTag> inputTagsEndProcessBlock2_;
    std::vector<edm::InputTag> inputTagsEndProcessBlock3_;
    std::vector<edm::InputTag> inputTagsEndProcessBlock4_;

    std::vector<edm::EDGetTokenT<IntProduct>> tokens_;
    std::vector<edm::EDGetTokenT<IntProduct>> tokensNotFound_;
    std::vector<edm::EDGetTokenT<edm::View<int>>> tokensView_;
    std::vector<edm::EDGetTokenT<UInt64Product>> tokensUInt64_;
    std::vector<edm::EDGetTokenT<IntProduct>> tokensEndLumi_;
    std::vector<edm::EDGetTokenT<IntProduct>> tokensEndRun_;
    std::vector<edm::EDGetTokenT<IntProduct>> tokensBeginProcessBlock_;
    std::vector<edm::EDGetTokenT<IntProduct>> tokensEndProcessBlock_;
    std::vector<edm::EDGetToken> tokensEndProcessBlock2_;
    std::vector<edm::EDGetTokenT<IntProduct>> tokensEndProcessBlock3_;
    std::vector<edm::EDGetTokenT<IntProduct>> tokensEndProcessBlock4_;

    edm::GetterOfProducts<IntProduct> getterOfProducts_;

  };  // class TestFindProduct

  //--------------------------------------------------------------------
  //
  // Implementation details

  TestFindProduct::TestFindProduct(edm::ParameterSet const& pset)
      : inputTags_(pset.getUntrackedParameter<std::vector<edm::InputTag>>("inputTags")),
        expectedSum_(pset.getUntrackedParameter<int>("expectedSum", 0)),
        sum_(0),
        inputTagsNotFound_(),
        getByTokenFirst_(pset.getUntrackedParameter<bool>("getByTokenFirst", false)),
        inputTagsView_(),
        runProducerParameterCheck_(pset.getUntrackedParameter<bool>("runProducerParameterCheck", false)),
        testGetterOfProducts_(pset.getUntrackedParameter<bool>("testGetterOfProducts", false)),
        getterOfProducts_(edm::ProcessMatch("*"), this, edm::InProcess) {
    if (testGetterOfProducts_) {
      callWhenNewProductsRegistered(getterOfProducts_);
    }
    std::vector<edm::InputTag> emptyTagVector;
    inputTagsNotFound_ = pset.getUntrackedParameter<std::vector<edm::InputTag>>("inputTagsNotFound", emptyTagVector);
    inputTagsView_ = pset.getUntrackedParameter<std::vector<edm::InputTag>>("inputTagsView", emptyTagVector);
    inputTagsUInt64_ = pset.getUntrackedParameter<std::vector<edm::InputTag>>("inputTagsUInt64", emptyTagVector);
    inputTagsEndLumi_ = pset.getUntrackedParameter<std::vector<edm::InputTag>>("inputTagsEndLumi", emptyTagVector);
    inputTagsEndRun_ = pset.getUntrackedParameter<std::vector<edm::InputTag>>("inputTagsEndRun", emptyTagVector);
    inputTagsBeginProcessBlock_ =
        pset.getUntrackedParameter<std::vector<edm::InputTag>>("inputTagsBeginProcessBlock", emptyTagVector);
    inputTagsEndProcessBlock_ =
        pset.getUntrackedParameter<std::vector<edm::InputTag>>("inputTagsEndProcessBlock", emptyTagVector);
    inputTagsEndProcessBlock2_ =
        pset.getUntrackedParameter<std::vector<edm::InputTag>>("inputTagsEndProcessBlock2", emptyTagVector);
    inputTagsEndProcessBlock3_ =
        pset.getUntrackedParameter<std::vector<edm::InputTag>>("inputTagsEndProcessBlock3", emptyTagVector);
    inputTagsEndProcessBlock4_ =
        pset.getUntrackedParameter<std::vector<edm::InputTag>>("inputTagsEndProcessBlock4", emptyTagVector);

    for (auto const& tag : inputTags_) {
      tokens_.push_back(consumes<IntProduct>(tag));
    }
    for (auto const& tag : inputTagsNotFound_) {
      tokensNotFound_.push_back(consumes<IntProduct>(tag));
    }
    for (auto const& tag : inputTagsView_) {
      tokensView_.push_back(consumes<edm::View<int>>(tag));
    }
    for (auto const& tag : inputTagsUInt64_) {
      tokensUInt64_.push_back(consumes<UInt64Product>(tag));
    }
    for (auto const& tag : inputTagsEndLumi_) {
      tokensEndLumi_.push_back(consumes<IntProduct, edm::InLumi>(tag));
    }
    for (auto const& tag : inputTagsEndRun_) {
      tokensEndRun_.push_back(consumes<IntProduct, edm::InRun>(tag));
    }
    for (auto const& tag : inputTagsBeginProcessBlock_) {
      tokensBeginProcessBlock_.push_back(consumes<IntProduct, edm::InProcess>(tag));
    }
    for (auto const& tag : inputTagsEndProcessBlock_) {
      tokensEndProcessBlock_.push_back(consumes<IntProduct, edm::InProcess>(tag));
    }
    for (auto const& tag : inputTagsEndProcessBlock2_) {
      tokensEndProcessBlock2_.push_back(consumes<IntProduct, edm::InProcess>(tag));
    }
    for (auto const& tag : inputTagsEndProcessBlock3_) {
      tokensEndProcessBlock3_.push_back(consumes<IntProduct, edm::InProcess>(tag));
    }
    for (auto const& tag : inputTagsEndProcessBlock4_) {
      tokensEndProcessBlock4_.push_back(consumes<IntProduct, edm::InProcess>(tag));
    }
  }

  TestFindProduct::~TestFindProduct() {}

  void TestFindProduct::analyze(edm::Event const& e, edm::EventSetup const&) {
    edm::Handle<IntProduct> h;
    edm::Handle<IntProduct> hToken;
    edm::Handle<edm::View<int>> hView;
    edm::Handle<edm::View<int>> hViewToken;

    std::vector<edm::EDGetTokenT<IntProduct>>::const_iterator iToken = tokens_.begin();
    for (std::vector<edm::InputTag>::const_iterator iter = inputTags_.begin(), iEnd = inputTags_.end(); iter != iEnd;
         ++iter, ++iToken) {
      if (getByTokenFirst_) {
        e.getByToken(*iToken, hToken);
        *hToken;
      }

      e.getByLabel(*iter, h);
      sum_ += h->value;

      e.getByToken(*iToken, hToken);
      if (h->value != hToken->value) {
        std::cerr << "TestFindProduct::analyze getByLabel and getByToken return inconsistent results " << std::endl;
        abort();
      }

      if (runProducerParameterCheck_) {
        edm::ParameterSet const* producerPset = edm::getProducerParameterSet(*hToken.provenance(), e.processHistory());
        int par = producerPset->getParameter<int>("ivalue");
        // These expected values are just from knowing the values in the
        // configuration files for this test.
        int expectedParameterValue = 3;
        if (!iter->process().empty()) {
          if (e.run() == 1) {
            expectedParameterValue = 1;
          } else {
            expectedParameterValue = 2;
          }
        }
        if (par != expectedParameterValue) {
          std::cerr << "TestFindProduct::analyze unexpected value from producer parameter set" << std::endl;
          abort();
        }
      }
    }
    iToken = tokensNotFound_.begin();
    for (std::vector<edm::InputTag>::const_iterator iter = inputTagsNotFound_.begin(), iEnd = inputTagsNotFound_.end();
         iter != iEnd;
         ++iter, ++iToken) {
      e.getByLabel(*iter, h);
      if (h.isValid()) {
        std::cerr << "TestFindProduct::analyze: getByLabel found a product that should not be found "
                  << h.provenance()->moduleLabel() << std::endl;
        abort();
      }

      e.getByToken(*iToken, hToken);
      if (hToken.isValid()) {
        std::cerr << "TestFindProduct::analyze: getByToken found a product that should not be found "
                  << hToken.provenance()->moduleLabel() << std::endl;
        abort();
      }
    }
    std::vector<edm::EDGetTokenT<edm::View<int>>>::const_iterator iTokenView = tokensView_.begin();
    for (std::vector<edm::InputTag>::const_iterator iter = inputTagsView_.begin(), iEnd = inputTagsView_.end();
         iter != iEnd;
         ++iter, ++iTokenView) {
      if (getByTokenFirst_) {
        e.getByToken(*iTokenView, hViewToken);
        *hViewToken;
      }

      e.getByLabel(*iter, hView);
      sum_ += hView->at(0);

      e.getByToken(*iTokenView, hViewToken);
      if (hView->at(0) != hViewToken->at(0)) {
        std::cerr << "TestFindProduct::analyze getByLabel and getByToken return inconsistent results " << std::endl;
        abort();
      }
    }

    // Get these also and add them into the sum
    edm::Handle<UInt64Product> h64;
    for (auto const& token : tokensUInt64_) {
      e.getByToken(token, h64);
      sum_ += h64->value;
    }
  }

  void TestFindProduct::beginLuminosityBlock(edm::LuminosityBlock const&, edm::EventSetup const&) {}

  void TestFindProduct::endLuminosityBlock(edm::LuminosityBlock const& lb, edm::EventSetup const&) {
    // Get these also and add them into the sum
    edm::Handle<IntProduct> h;
    for (auto const& token : tokensEndLumi_) {
      lb.getByToken(token, h);
      sum_ += h->value;
    }
  }

  void TestFindProduct::beginRun(edm::Run const&, edm::EventSetup const&) {}

  void TestFindProduct::endRun(edm::Run const& run, edm::EventSetup const&) {
    // Get these also and add them into the sum
    edm::Handle<IntProduct> h;
    for (auto const& token : tokensEndRun_) {
      run.getByToken(token, h);
      sum_ += h->value;
    }
  }

  void TestFindProduct::beginProcessBlock(edm::ProcessBlock const& processBlock) {
    for (auto const& token : tokensBeginProcessBlock_) {
      sum_ += processBlock.get(token).value;
    }
    if (testGetterOfProducts_) {
      std::vector<edm::Handle<IntProduct>> handles;
      getterOfProducts_.fillHandles(processBlock, handles);
      for (auto const& intHandle : handles) {
        sum_ += intHandle->value;
      }
    }
  }

  void TestFindProduct::endProcessBlock(edm::ProcessBlock const& processBlock) {
    std::vector<int> values;
    for (auto const& token : tokensEndProcessBlock_) {
      int value = processBlock.get(token).value;
      values.push_back(value);
      sum_ += value;
    }
    edm::Handle<IntProduct> h;
    unsigned int i = 0;
    for (auto val : values) {
      if (i < tokensEndProcessBlock2_.size()) {
        processBlock.getByToken(tokensEndProcessBlock2_[i], h);
        if (h->value != val + 2) {
          throw cms::Exception("TestFail") << "TestFindProduct::endProcessBlock 2, received unexpected value";
        }
      }
      if (i < tokensEndProcessBlock3_.size()) {
        processBlock.getByToken(tokensEndProcessBlock3_[i], h);
        if (h->value != val + 3) {
          throw cms::Exception("TestFail") << "TestFindProduct::endProcessBlock 3, received unexpected value";
        }
      }
      if (i < tokensEndProcessBlock4_.size()) {
        h = processBlock.getHandle(tokensEndProcessBlock4_[i]);
        if (h->value != val + 4) {
          throw cms::Exception("TestFail") << "TestFindProduct::endProcessBlock 4, received unexpected value";
        }
      }
      ++i;
    }
    if (testGetterOfProducts_) {
      std::vector<edm::Handle<IntProduct>> handles;
      getterOfProducts_.fillHandles(processBlock, handles);
      for (auto const& intHandle : handles) {
        sum_ += intHandle->value;
      }
    }
  }

  void TestFindProduct::endJob() {
    std::cout << "TestFindProduct sum = " << sum_ << std::endl;
    if (expectedSum_ != 0 && sum_ != expectedSum_) {
      throw cms::Exception("TestFail")
          << "TestFindProduct::endJob - Sum of test object values does not equal expected value";
    }
  }
}  // namespace edmtest

using edmtest::TestFindProduct;
DEFINE_FWK_MODULE(TestFindProduct);
