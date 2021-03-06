#include <iostream>
#include <vector>
#include <memory>
#include "RecoEgamma/EgammaPhotonAlgos/interface/ConversionTrackEcalImpactPoint.h"
// Framework
//

//
#include <vector>
#include <map>

constexpr float epsilon = 0.001;

/** Hard-wired numbers defining the surfaces on which the crystal front faces lie. */
constexpr float barrelRadius = 129.f;       // p81, p50, ECAL TDR
constexpr float barrelHalfLength = 270.9f;  // p81, p50, ECAL TDR
constexpr float endcapRadius = 171.1f;      // fig 3.26, p81, ECAL TDR
constexpr float endcapZ = 320.5f;           // fig 3.26, p81, ECAL TDR

static BoundCylinder* initBarrel() {
  Surface::RotationType rot;  // unit rotation matrix

  return new Cylinder(
      barrelRadius,
      Surface::PositionType(0, 0, 0),
      rot,
      new SimpleCylinderBounds(barrelRadius - epsilon, barrelRadius + epsilon, -barrelHalfLength, barrelHalfLength));
}

static BoundDisk* initNegative() {
  Surface::RotationType rot;  // unit rotation matrix
  return new BoundDisk(
      Surface::PositionType(0, 0, -endcapZ), rot, new SimpleDiskBounds(0, endcapRadius, -epsilon, epsilon));
}

static BoundDisk* initPositive() {
  Surface::RotationType rot;  // unit rotation matrix

  return new BoundDisk(
      Surface::PositionType(0, 0, endcapZ), rot, new SimpleDiskBounds(0, endcapRadius, -epsilon, epsilon));
}

const ReferenceCountingPointer<BoundCylinder> ConversionTrackEcalImpactPoint::theBarrel_ = initBarrel();
const ReferenceCountingPointer<BoundDisk> ConversionTrackEcalImpactPoint::theNegativeEtaEndcap_ = initNegative();
const ReferenceCountingPointer<BoundDisk> ConversionTrackEcalImpactPoint::thePositiveEtaEndcap_ = initPositive();

ConversionTrackEcalImpactPoint::ConversionTrackEcalImpactPoint(const MagneticField* field)
    :

      theMF_(field) {
  forwardPropagator_ = new PropagatorWithMaterial(dir_ = alongMomentum, 0.000511, theMF_);
}

ConversionTrackEcalImpactPoint::~ConversionTrackEcalImpactPoint() { delete forwardPropagator_; }

std::vector<math::XYZPointF> ConversionTrackEcalImpactPoint::find(
    const std::vector<reco::TransientTrack>& tracks, const edm::Handle<edm::View<reco::CaloCluster> >& bcHandle) {
  std::vector<math::XYZPointF> result;
  //
  matchingBC_.clear();

  std::vector<reco::CaloClusterPtr> matchingBC(2);

  //

  int iTrk = 0;
  for (auto const& track : tracks) {
    math::XYZPointF ecalImpactPosition(0., 0., 0.);
    const TrajectoryStateOnSurface myTSOS = track.innermostMeasurementState();
    if (!(myTSOS.isValid()))
      continue;

    stateAtECAL_ = forwardPropagator_->propagate(myTSOS, barrel());

    if (!stateAtECAL_.isValid() || (stateAtECAL_.isValid() && fabs(stateAtECAL_.globalPosition().eta()) > 1.479)) {
      if (track.innermostMeasurementState().globalPosition().eta() > 0.) {
        stateAtECAL_ = forwardPropagator_->propagate(myTSOS, positiveEtaEndcap());

      } else {
        stateAtECAL_ = forwardPropagator_->propagate(myTSOS, negativeEtaEndcap());
      }
    }

    if (stateAtECAL_.isValid())
      ecalImpactPosition = stateAtECAL_.globalPosition();

    result.push_back(ecalImpactPosition);

    if (stateAtECAL_.isValid()) {
      int goodBC = 0;
      float bcDistanceToTrack = 9999;
      reco::CaloClusterPtr matchedBCItr;
      int ibc = 0;
      goodBC = 0;

      for (auto const& bc : *bcHandle) {
        float dEta = bc.position().eta() - ecalImpactPosition.eta();
        float dPhi = bc.position().phi() - ecalImpactPosition.phi();
        if (sqrt(dEta * dEta + dPhi * dPhi) < bcDistanceToTrack) {
          goodBC = ibc;
          bcDistanceToTrack = sqrt(dEta * dEta + dPhi * dPhi);
        }
        ibc++;
      }

      matchingBC[iTrk] = bcHandle->ptrAt(goodBC);
    }

    iTrk++;
  }

  matchingBC_ = matchingBC;

  return result;
}
